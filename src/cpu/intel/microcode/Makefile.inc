################################################################################
## One small file with the awesome super-power of updating the CPU microcode
## directly from CBFS. You have been WARNED!!!
################################################################################
ramstage-$(CONFIG_SUPPORT_CPU_UCODE_IN_CBFS) += microcode.c
romstage-$(CONFIG_SUPPORT_CPU_UCODE_IN_CBFS) += microcode.c
