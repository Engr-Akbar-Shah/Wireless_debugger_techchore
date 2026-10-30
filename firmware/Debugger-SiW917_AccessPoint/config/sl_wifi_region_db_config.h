/***************************************************************************//**
 * @author :  Engineer Akbar Shah
 * @date   :  2024-06-10
 * @file   :  sl_wifi_region_db_config.h
 * @brief  :        Stores the Wi-Fi region database configuration values.
 ******************************************************************************/



#pragma once

// Define default region-specific configurations for 2.4 GHz and 5 GHz bands
const sli_wifi_set_region_ap_request_t default_US_region_2_4GHZ_configurations = {
  .set_region_code_from_user_cmd = SET_REGION_CODE_FROM_USER,
  .country_code                  = "US ",
  .no_of_rules                   = 1,
  .channel_info[0]               = { .first_channel = 1, .no_of_channels = 11, .max_tx_power = 30 }
};

const sli_wifi_set_region_ap_request_t default_US_region_5GHZ_configurations = {
  .set_region_code_from_user_cmd = SET_REGION_CODE_FROM_USER,
  .country_code                  = "US ",
  .no_of_rules                   = 5,
  .channel_info[0]               = { .first_channel = 36, .no_of_channels = 4, .max_tx_power = 16 },
  .channel_info[1]               = { .first_channel = 52, .no_of_channels = 4, .max_tx_power = 23 },
  .channel_info[2]               = { .first_channel = 100, .no_of_channels = 5, .max_tx_power = 23 },
  .channel_info[3]               = { .first_channel = 132, .no_of_channels = 3, .max_tx_power = 23 },
  .channel_info[4]               = { .first_channel = 149, .no_of_channels = 5, .max_tx_power = 29 }
};

// Define default configurations for the European region for 2.4 GHz and 5 GHz bands
const sli_wifi_set_region_ap_request_t default_EU_region_2_4GHZ_configurations = {
  .set_region_code_from_user_cmd = SET_REGION_CODE_FROM_USER,
  .country_code                  = "EU ",
  .no_of_rules                   = 1,
  .channel_info[0]               = { .first_channel = 1, .no_of_channels = 13, .max_tx_power = 20 }
};

const sli_wifi_set_region_ap_request_t default_EU_region_5GHZ_configurations = {
  .set_region_code_from_user_cmd = SET_REGION_CODE_FROM_USER,
  .country_code                  = "EU ",
  .no_of_rules                   = 3,
  .channel_info[0]               = { .first_channel = 36, .no_of_channels = 4, .max_tx_power = 23 },
  .channel_info[1]               = { .first_channel = 52, .no_of_channels = 4, .max_tx_power = 23 },
  .channel_info[2]               = { .first_channel = 100, .no_of_channels = 11, .max_tx_power = 30 }
};

// Define default configurations for the Japanese region for 2.4 GHz and 5 GHz bands
const sli_wifi_set_region_ap_request_t default_JP_region_2_4GHZ_configurations = {
  .set_region_code_from_user_cmd = SET_REGION_CODE_FROM_USER,
  .country_code                  = "JP ",
  .no_of_rules                   = 1,
  .channel_info[0]               = { .first_channel = 1, .no_of_channels = 14, .max_tx_power = 20 }
};

const sli_wifi_set_region_ap_request_t default_JP_region_5GHZ_configurations = {
  .set_region_code_from_user_cmd = SET_REGION_CODE_FROM_USER,
  .country_code                  = "JP ",
  .no_of_rules                   = 3,
  .channel_info[0]               = { .first_channel = 36, .no_of_channels = 4, .max_tx_power = 20 },
  .channel_info[1]               = { .first_channel = 52, .no_of_channels = 4, .max_tx_power = 20 },
  .channel_info[2]               = { .first_channel = 100, .no_of_channels = 11, .max_tx_power = 30 }
};

// Define default configurations for the Korean region for 2.4 GHz and 5 GHz bands
const sli_wifi_set_region_ap_request_t default_KR_region_2_4GHZ_configurations = {
  .set_region_code_from_user_cmd = SET_REGION_CODE_FROM_USER,
  .country_code                  = "KR ",
  .no_of_rules                   = 1,
  .channel_info[0]               = { .first_channel = 1, .no_of_channels = 13, .max_tx_power = 23 }
};

const sli_wifi_set_region_ap_request_t default_KR_region_5GHZ_configurations = {
  .set_region_code_from_user_cmd = SET_REGION_CODE_FROM_USER,
  .country_code                  = "KR ",
  .no_of_rules                   = 4,
  .channel_info[0]               = { .first_channel = 36, .no_of_channels = 4, .max_tx_power = 23 },
  .channel_info[1]               = { .first_channel = 52, .no_of_channels = 4, .max_tx_power = 20 },
  .channel_info[2]               = { .first_channel = 100, .no_of_channels = 11, .max_tx_power = 20 },
  .channel_info[3]               = { .first_channel = 149, .no_of_channels = 5, .max_tx_power = 23 }
};

// Define default configurations for the Singapore region for 2.4 GHz and 5 GHz bands
const sli_wifi_set_region_ap_request_t default_SG_region_2_4GHZ_configurations = {
  .set_region_code_from_user_cmd = SET_REGION_CODE_FROM_USER,
  .country_code                  = "SG ",
  .no_of_rules                   = 1,
  .channel_info[0]               = { .first_channel = 1, .no_of_channels = 13, .max_tx_power = 27 }
};

const sli_wifi_set_region_ap_request_t default_SG_region_5GHZ_configurations = {
  .set_region_code_from_user_cmd = SET_REGION_CODE_FROM_USER,
  .country_code                  = "SG ",
  .no_of_rules                   = 5,
  .channel_info[0]               = { .first_channel = 36, .no_of_channels = 4, .max_tx_power = 16 },
  .channel_info[1]               = { .first_channel = 52, .no_of_channels = 4, .max_tx_power = 23 },
  .channel_info[2]               = { .first_channel = 100, .no_of_channels = 5, .max_tx_power = 23 },
  .channel_info[3]               = { .first_channel = 132, .no_of_channels = 3, .max_tx_power = 23 },
  .channel_info[4]               = { .first_channel = 149, .no_of_channels = 4, .max_tx_power = 29 }
};

// Define default configurations for the China region for 2.4 GHz and 5 GHz bands
const sli_wifi_set_region_ap_request_t default_CN_region_2_4GHZ_configurations = {
  .set_region_code_from_user_cmd = SET_REGION_CODE_FROM_USER,
  .country_code                  = "CN ",
  .no_of_rules                   = 1,
  .channel_info[0]               = { .first_channel = 1, .no_of_channels = 13, .max_tx_power = 20 }
};

const sli_wifi_set_region_ap_request_t default_CN_region_5GHZ_configurations = {
  .set_region_code_from_user_cmd = SET_REGION_CODE_FROM_USER,
  .country_code                  = "CN ",
  .no_of_rules                   = 2,
  .channel_info[0]               = { .first_channel = 36, .no_of_channels = 9, .max_tx_power = 20 },
  .channel_info[4]               = { .first_channel = 149, .no_of_channels = 5, .max_tx_power = 33 }
};

