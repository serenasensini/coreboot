bootblock-y += bootblock.c
bootblock-y += ec.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-y += boardid.c

ramstage-y += boardid.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-y += ec.c
ramstage-y += mainboard.c

verstage-$(CONFIG_CHROMEOS) += chromeos.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c
