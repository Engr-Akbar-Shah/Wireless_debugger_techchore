#!/usr/bin/env python3
"""New Pico-path functional checks only; never flashes or runs endurance tests."""
import datetime,hashlib,json,time
from pathlib import Path
import serial
ROOT=Path(__file__).resolve().parents[1]
REPORT=ROOT/'hardware-result.json'
result={'started_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
        'image_manifest':json.loads((ROOT/'build-manifest.json').read_text()),
        'scope':'New Pico USB line-coding, short binary echo and USB DTR/RTS ROM identification; no endurance qualification',
        'steps':[],'passed':False}
status_lines=[]

def save():
    result['status_lines']=status_lines[-500:]
    REPORT.write_text(json.dumps(result,indent=2)+'\n')

def identify():
    deadline=time.monotonic()+10
    while time.monotonic()<deadline:
        links=list(Path('/dev/serial/by-id').glob('*Wireless_UART*'))
        if any(str(p).endswith('-if00') for p in links) and any(str(p).endswith('-if02') for p in links):break
        time.sleep(.2)
    if not links:links=list(Path('/dev/serial/by-id').glob('*Pico_bridge*'))
    data=next(p for p in links if str(p).endswith('-if00'))
    status=next(p for p in links if str(p).endswith('-if02'))
    return str(data),str(status)

def open_data(path):
    port=serial.Serial(port=None,baudrate=115200,timeout=.05,write_timeout=2,exclusive=True)
    port.dtr=False;port.rts=False;port.port=path;port.open();return port

def status_wait(console,baud=None,parity=None,dtr=None,rts=None,timeout=12):
    end=time.monotonic()+timeout;partial=bytearray();last_query=0
    while time.monotonic()<end:
        if time.monotonic()-last_query>.3:console.write(b'STATUS\n');last_query=time.monotonic()
        partial.extend(console.read(max(1,console.in_waiting)))
        while b'\n' in partial:
            line,_,rest=partial.partition(b'\n');partial[:]=rest
            text=line.decode('ascii','replace').strip();status_lines.append(text)
            if not text.startswith('STATUS '):continue
            fields=dict(item.split('=',1) for item in text.split()[1:] if '=' in item)
            if fields.get('ready')!='1':continue
            if baud is not None and fields.get('baud')!=str(baud):continue
            if parity is not None and fields.get('parity')!=parity:continue
            if dtr is not None and fields.get('dtr')!=str(int(dtr)):continue
            if rts is not None and fields.get('rts')!=str(int(rts)):continue
            return fields
    raise TimeoutError('Pico applied-state timeout; last status lines: '+repr(status_lines[-12:]))

def fixture_baud(port,baud):
    port.reset_input_buffer();port.write(f'BAUD {baud}\n'.encode())
    line=port.readline().decode('ascii','replace').strip();parts=line.split()
    if len(parts)!=4 or parts[0]!='OK' or int(parts[1])!=baud:raise RuntimeError('STM32 fixture control failed: '+line)
    return {'nominal':int(parts[1]),'driver_requested':int(parts[2]),'brr':int(parts[3])}

def exact_read(port,n,timeout):
    got=bytearray();end=time.monotonic()+timeout
    while len(got)<n and time.monotonic()<end:got.extend(port.read(n-len(got)))
    return bytes(got)

def rom_sync_ack(port,timeout=3,max_prefix=512):
    """Only ROM synchronization permits a bounded, recorded reset prefix.

    STM32 TX can be high impedance until ROM autobaud establishes its UART.
    Old reset-transition bytes may still be in the wireless pipeline when7F
    is sent. Never apply this filtering to ordinary DATA or GET_ID responses.
    """
    prefix=bytearray();end=time.monotonic()+timeout
    while time.monotonic()<end:
        byte=port.read(1)
        if not byte:continue
        if byte==b'\x79':return prefix.hex()
        if byte==b'\x1f':raise RuntimeError('ROM sync NACK; prefix='+prefix.hex())
        prefix.extend(byte)
        if len(prefix)>max_prefix:raise RuntimeError('ROM reset prefix exceeded limit: '+prefix.hex())
    raise RuntimeError('ROM sync ACK missing; reset prefix='+prefix.hex())

def echo(port,n=4096):
    payload=bytes((i*73+(i>>8)*17)&255 for i in range(n));got=bytearray();start=time.monotonic()
    stale=port.read(port.in_waiting)
    if stale:raise RuntimeError('Unexpected stale DATA before echo: '+stale[:32].hex())
    for i in range(0,n,256):
        block=payload[i:i+256];port.write(block);port.flush();received=exact_read(port,len(block),max(3,len(block)*20/port.baudrate))
        if received!=block:raise RuntimeError(f'Echo failure at offset{i}: expected{len(block)}, got{len(received)}, prefix={received[:32].hex()}')
        got.extend(received)
    time.sleep(.15)
    if port.in_waiting:raise RuntimeError('Extra bytes after exact echo')
    return {'bytes_each_direction':n,'sha256':hashlib.sha256(got).hexdigest(),'elapsed_seconds':round(time.monotonic()-start,3),'passed':True}

def rom_identify(data,console):
    # Opening at 8E1 automatically selects the STM32 bootloader (Pico 3.12.1):
    # reset, BOOT0 high, release. STATUS shows logical dtr=reset, rts=BOOT0.
    data.reset_input_buffer()
    data.baudrate=115200;data.parity=serial.PARITY_EVEN
    status_wait(console,baud=115200,parity='E',dtr=False,rts=True)
    time.sleep(.15)
    transition=data.read(data.in_waiting).hex()
    data.write(b'\x7f');data.flush()
    late_transition=rom_sync_ack(data)
    data.write(b'\x02\xfd');data.flush()
    ack=exact_read(data,1,3)
    if ack!=b'\x79':raise RuntimeError('ROM GetID ACK missing: '+ack.hex())
    size=exact_read(data,1,3)
    if len(size)!=1:raise RuntimeError('ROM GetID length missing')
    chip=exact_read(data,size[0]+1,3);final=exact_read(data,1,3)
    if len(chip)!=size[0]+1 or final!=b'\x79':raise RuntimeError('Incomplete ROM GetID response')
    return {'sync_ACK':'79','command':'GET_ID read-only','chip_id_hex':chip.hex(),'reset_transition_bytes_hex':transition,'late_reset_prefix_hex':late_transition,'passed':True}

def restore_app(data,console,fixture):
    # Returning to 8N1 automatically resets the STM32 into its application.
    data.baudrate=115200;data.parity=serial.PARITY_NONE
    status_wait(console,dtr=False,rts=False,baud=115200,parity='N')
    time.sleep(.2);state=fixture_baud(fixture,115200)
    time.sleep(.15);transition=data.read(data.in_waiting).hex()
    check=echo(data,256)
    return {'fixture':state,'echo':check,'reset_transition_bytes_hex':transition,'passed':True}


def main():
    save();data=console=fixture=None
    try:
        path,status_path=identify();result.update(data_port=path,status_port=status_path)
        console=serial.Serial(status_path,115200,timeout=.05,write_timeout=1,exclusive=True)
        data=open_data(path)
        fixture=serial.Serial('/dev/serial/by-id/usb-STMicroelectronics_STM32_STLink_066CFF535767864967152953-if02',115200,timeout=2,write_timeout=1,exclusive=True)
        status_wait(console,dtr=False,rts=False,timeout=65)
        time.sleep(.3)
        result['startup_bytes_before_payload_hex']=data.read(data.in_waiting).hex()
        fd=data.fileno();result['same_open_COM_handle']=True
        for baud in [115200,921600,9600,57600,230400,460800,115200]:
            target=fixture_baud(fixture,baud);data.baudrate=baud
            applied=status_wait(console,baud=baud,parity='N',dtr=False,rts=False)
            check=echo(data);entry={'type':'baud_change','baud':baud,'fixture':target,'Pico_ACK':applied,'binary':check}
            assert data.fileno()==fd;result['steps'].append(entry);save()
            print(f'PASS USB baud{baud}:4096exact bytes each way, same open COM handle',flush=True)
        entry=rom_identify(data,console);result['steps'].append({'type':'USB_DTR_RTS_ROM',**entry});save()
        print('PASS USB DTR/RTS: ROM sync and read-only chip identification '+entry['chip_id_hex'],flush=True)
        result['steps'].append({'type':'restore_application',**restore_app(data,console,fixture)})
        result['passed']=True;print('PASS application restored at1152008N1 with exact echo',flush=True)
    except Exception as exc:
        result['error']=type(exc).__name__+': '+str(exc);print(result['error'],flush=True)
        if console:
            try:
                console.write(b'STATUS\n');end=time.monotonic()+1;extra=bytearray()
                while time.monotonic()<end:extra.extend(console.read(max(1,console.in_waiting)))
                status_lines.extend(extra.decode('ascii','replace').splitlines())
            except Exception:pass
    finally:
        if data:
            try:data.dtr=False;data.rts=False
            except Exception:pass
            data.close()
        if console:console.close()
        if fixture:fixture.close()
        result['finished_utc']=datetime.datetime.now(datetime.timezone.utc).isoformat();save()
    return 0 if result['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
