# 当前文件所在目录
LOCAL_PATH := $(call my-dir)

ZIGBEE_HOST_USER_SRC_ROOT_DIR := $(LOCAL_PATH)/slabs/v2.2

ZIGBEE_HOST_CONFIG_DEFINES = \
	-DCONFIGURATION_HEADER=\"protocol/zigbee/app/framework/util/config.h\" \
	-DZA_GENERATED_HEADER=\"Z3GatewayHost.h\" \
	-DATTRIBUTE_STORAGE_CONFIGURATION=\"Z3GatewayHost_endpoint_config.h\" \
	-DPLATFORM_HEADER=\"platform/base/hal/micro/unix/compiler/gcc.h\" \
	-DBOARD_HEADER=\"app/builder/Z3GatewayHost/Z3GatewayHost_board.h\" \
	-DEMBER_AF_API_EMBER_TYPES=\"stack/include/ember-types.h\" \
	-DEMBER_AF_API_DEBUG_PRINT=\"app/framework/util/print.h\" \
	-DEMBER_AF_API_AF_HEADER=\"app/framework/include/af.h\" \
	-DEMBER_AF_API_AF_SECURITY_HEADER=\"app/framework/security/af-security.h\" \


ZIGBEE_HOST_DEFINES = \
	$(ZIGBEE_HOST_CONFIG_DEFINES) \
	-DUNIX \
	-DUNIX_HOST \
	-DPHY_NULL \
	-DEZSP_HOST \
	-DGATEWAY_APP \
	-DBOARD_HOST \
	-DEM_AF_TEST_HARNESS_CODE \
	-DEM_AF_LINK_M \
	-DEM_AF_LINK_PTHREAD \
	-DEMBER_STACK_ZIGBEE \
	-DEZSP_ASH \
	-DGENERATE_LIBRARY \
	-DEMBER_AF_PLUGIN_FRAGMENTATION_MAX_OUTGOING_PACKETS=200 \
	-DEMBER_AF_PLUGIN_FRAGMENTATION_MAX_INCOMING_PACKETS=200 \
	-DNO_READLINE=1 \
	-DTY_FAST_JOIN \
	-D_GNU_SOURCE \

# 未定义的默认为小端
ifneq ($(LITTLE_END), 0)
ZIGBEE_HOST_DEFINES += -D__LITTLE_ENDIAN__
endif

ZIGBEE_HOST_USER_INCLUDES = \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR) \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/uECC \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/stack \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/util \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/util/common \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/util/ezsp \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/util/serial \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/util/zigbee-framework \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/cli \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/include \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/security \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/plugin \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/micro/generic \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/micro/generic/aes \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/micro/unix/host \
	-I$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/micro/unix/host/board \

ZIGBEE_HOST_USER_SRC = \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/micro/generic/led-stub.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/micro/generic/mem-util.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/micro/generic/ash-common.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/micro/generic/crc.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/micro/generic/endian.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/micro/generic/random.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/micro/generic/system-timer.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/micro/unix/host/micro.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/micro/unix/host/token-def-unix.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/micro/unix/host/token.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/micro/generic/aes/rijndael-alg-fst.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/micro/generic/aes/rijndael-api-fst.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/plugin/antenna-stub/antenna-stub.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/plugin/buzzer-stub/buzzer-stub.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/hal/plugin/serial/ember-printf-convert.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/platform/base/phy/simulation/aes-software.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/cli/core-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/cli/network-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/cli/option-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/cli/plugin-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/cli/security-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/cli/zcl-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/cli/zdo-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/security/af-node.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/security/af-security-common.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/security/af-trust-center.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/security/crypto-state.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util/af-event.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util/af-main-common.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util/af-main-host.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util/attribute-size.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util/attribute-storage.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util/attribute-table.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util/client-api.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util/message.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util/multi-network.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util/print.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util/print-formatter.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util/process-cluster-message.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util/process-global-message.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util/service-discovery-common.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util/service-discovery-host.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util/time-util.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/util/util.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/../util/common/library.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/../util/serial/command-interpreter2.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/../util/zigbee-framework/zigbee-device-common.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/../util/zigbee-framework/zigbee-device-host.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/../../stack/framework/event-control.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/address-table/address-table.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/address-table/address-table-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/basic/basic.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/color-control-server/color-control-server.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/concentrator/concentrator-support.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/concentrator/concentrator-support-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/concentrator/source-route-common.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/concentrator/source-route-host.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/counters/counters-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/counters/counters-ota-host.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/counters/counters-host.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/device-table/device-table.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/device-table/device-table-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/device-table/device-table-discovery.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/device-table/device-table-tracking.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ezmode-commissioning/ez-mode.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ezmode-commissioning/ez-mode-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/form-and-join/form-and-join-afv2.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/util/common/form-and-join.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/util/common/form-and-join-host-adapter.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/green-power-client/green-power-client.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/green-power-client/green-power-client-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/green-power-common/green-power-common.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/stack/gp/gp-util.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/green-power-server/green-power-server.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/green-power-server/green-power-server-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/green-power-server/green-power-security.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/green-power-server/green-power-crypto.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/heartbeat/heartbeat.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ias-zone-client/ias-zone-client.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ias-zone-client/ias-zone-client-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/identify/identify.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/identify/identify-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/identify-feedback/identify-feedback.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/level-control/level-control.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/manufacturing-library-cli/manufacturing-library-cli-host.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/network-creator/network-creator.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/network-creator/network-creator-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/network-creator-security/network-creator-security.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/network-creator-security/network-creator-security-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/network-find/network-find.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/network-find/network-find-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/on-off/on-off.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-bootload/ota-bootload-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-bootload/ota-bootload-ncp.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-bootload/ota-bootload-ncp-uart.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-bootload/ota-bootload-ncp-spi.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-bootload/ota-bootload-xmodem.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-client/ota-client.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-client/ota-client-signature-verify.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-client/ota-client-page-request.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-client/ota-client-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-client-policy/ota-client-policy.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-common/ota-common.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-server/ota-server.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-server/ota-server-page-request.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-server/ota-server-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-server-policy/ota-server-policy.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-storage-common/ota-storage-common.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-storage-common/ota-storage-common-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/ota-storage-posix-filesystem/ota-storage-linux.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/poll-control-client/poll-control-client.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/poll-control-client/poll-control-client-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/reporting/reporting.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/reporting/reporting-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/reporting/reporting-default-configuration.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/scan-dispatch/scan-dispatch.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/simple-metering-client/simple-metering-client.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/simple-metering-client/simple-metering-client-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/stack-diagnostics/stack-diagnostics.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/test-harness/test-harness.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/test-harness/read-write-attributes.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/test-harness/test-harness-host.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/time-server/time-server.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/trust-center-backup/trust-center-backup.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/trust-center-backup/trust-center-backup-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/trust-center-backup/trust-center-backup-cli-posix.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/trust-center-backup/trust-center-backup-posix.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/trust-center-nwk-key-update-broadcast/trust-center-nwk-key-update-broadcast.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/trust-center-nwk-key-update-periodic/trust-center-nwk-key-update-periodic.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/trust-center-nwk-key-update-unicast/trust-center-nwk-key-update-unicast.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/util/ezsp/ezsp-callbacks.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/util/ezsp/ezsp-enum-decode.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/util/ezsp/ezsp-frame-utilities.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/util/ezsp/ezsp.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/ezsp-host/ezsp-host-io.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/ezsp-host/ezsp-host-queues.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/ezsp-host/ezsp-host-ui.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/util/ezsp/serial-interface-uart.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/ezsp-host/ash/ash-host-ui.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/ezsp-host/ash/ash-host.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin-host/file-descriptor-dispatch/file-descriptor-dispatch.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin-host/gateway/gateway-support.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin-host/gateway/backchannel-support.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin-host/gateway/gateway-support-cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin-host/ncp-configuration/ncp-configuration.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/util/ezsp/secure-ezsp-stub.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/util/serial/linux-serial.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/protocol/zigbee/app/framework/plugin/aes-cmac/aes-cmac.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/util/plugin/plugin-common/linked-list/linked-list.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/afv2-bookkeeping.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/call-command-handler.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/callback-stub.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/stack-handler-stub.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/cli.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/Z3GatewayHost_callbacks.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/trust_center_swap_out_callbacks.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/tuya_z3_support_manu.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/tuya_z3_base.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/dev_table_tmp.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/tuya_z3_api_tmp.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/tuya_z3_msg_api.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/tuya_z3_adapter.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/z3_custom_frame.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/z3_slabs.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/tuya_z3_com_api.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/uECC/uECC.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/tuya_z3_uf_file.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/tuya_z3_host_storage.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/tuya_z3_zigbee_api.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/tuya_z3_whitelist.c \
	$(ZIGBEE_HOST_USER_SRC_ROOT_DIR)/app/builder/Z3GatewayHost/tuya_z3_host_flowctrl.c \


#---------------------------------------

# 清除 LOCAL_xxx 变量
include $(CLEAR_VARS)

# 当前模块名
LOCAL_MODULE := $(notdir $(LOCAL_PATH))

# 模块源代码
LOCAL_SRC_FILES := $(ZIGBEE_HOST_USER_SRC)

# 模块的 CFLAGS
LOCAL_CFLAGS := $(ZIGBEE_HOST_DEFINES) $(ZIGBEE_HOST_USER_INCLUDES) -I$(LOCAL_PATH)/include -std=gnu99

# 生成静态库
include $(BUILD_STATIC_LIBRARY)

# 生成动态库
include $(BUILD_SHARED_LIBRARY)

#---------------------------------------
