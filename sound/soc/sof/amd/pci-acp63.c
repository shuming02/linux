// SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause)
//
// This file is provided under a dual BSD/GPLv2 license. When using or
// redistributing this file, you may do so under either license.
//
// Copyright(c) 2023 Advanced Micro Devices, Inc. All rights reserved.
//
// Authors: Vijendar Mukunda <Vijendar.Mukunda@amd.com>

/*.
 * PCI interface for ACP6.3 device
 */

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <sound/sof.h>
#include <sound/soc-acpi.h>

#include "../ops.h"
#include "../sof-pci-dev.h"
#include "../../amd/mach-config.h"
#include "acp.h"
#include "acp-dsp-offset.h"

#define ACP6X_FUTURE_REG_ACLK_0		0x1854
#define ACP6x_REG_START			0x1240000
#define ACP6x_REG_END			0x125C000

static const struct sof_amd_acp_desc acp63_chip_info = {
	.pgfsm_base	= ACP6X_PGFSM_BASE,
	.ext_intr_enb = ACP6X_EXTERNAL_INTR_ENB,
	.ext_intr_cntl = ACP6X_EXTERNAL_INTR_CNTL,
	.ext_intr_stat	= ACP6X_EXT_INTR_STAT,
	.ext_intr_stat1	= ACP6X_EXT_INTR_STAT1,
	.acp_error_stat = ACP6X_ERROR_STATUS,
	.acp_sw0_i2s_err_reason = ACP6X_SW0_I2S_ERROR_REASON,
	.dsp_intr_base	= ACP6X_DSP_SW_INTR_BASE,
	.sram_pte_offset = ACP6X_SRAM_PTE_OFFSET,
	.hw_semaphore_offset = ACP6X_AXI2DAGB_SEM_0,
	.fusion_dsp_offset = ACP6X_DSP_FUSION_RUNSTALL,
	.probe_reg_offset = ACP6X_FUTURE_REG_ACLK_0,
	.sdw_max_link_count = ACP6X_SDW_MAX_MANAGER_COUNT,
	.sdw_acpi_dev_addr = SDW_ACPI_ADDR_ACP63,
	.reg_start_addr = ACP6x_REG_START,
	.reg_end_addr = ACP6x_REG_END,
};

static const struct sof_dev_desc acp63_desc = {
	.machines		= snd_soc_acpi_amd_acp63_sof_machines,
	.alt_machines           = snd_soc_acpi_amd_acp63_sof_sdw_machines,
	.resindex_lpe_base	= 0,
	.resindex_pcicfg_base	= -1,
	.resindex_imr_base	= -1,
	.irqindex_host_ipc	= -1,
	.chip_info		= &acp63_chip_info,
	.ipc_supported_mask     = BIT(SOF_IPC_TYPE_3),
	.ipc_default            = SOF_IPC_TYPE_3,
	.default_fw_path	= {
		[SOF_IPC_TYPE_3] = "amd/sof",
	},
	.default_tplg_path	= {
		[SOF_IPC_TYPE_3] = "amd/sof-tplg",
	},
	.default_fw_filename	= {
		[SOF_IPC_TYPE_3] = "sof-acp_6_3.ri",
	},
	.nocodec_tplg_filename	= "sof-acp.tplg",
	.ops			= &sof_acp63_ops,
	.ops_init		= sof_acp63_ops_init,
};

static int acp63_pci_probe(struct pci_dev *pci, const struct pci_device_id *pci_id)
{
	unsigned int flag;

	if (pci->revision != ACP63_PCI_ID)
		return -ENODEV;

	flag = snd_amd_acp_find_config(pci);
	if (flag != FLAG_AMD_SOF && flag != FLAG_AMD_SOF_ONLY_DMIC)
		return -ENODEV;

	return sof_pci_probe(pci, pci_id);
};

static void acp63_pci_remove(struct pci_dev *pci)
{
	sof_pci_remove(pci);
}

/* PCI IDs */
static const struct pci_device_id acp63_pci_ids[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_AMD, ACP_PCI_DEV_ID),
	.driver_data = (unsigned long)&acp63_desc},
	{ 0, }
};
MODULE_DEVICE_TABLE(pci, acp63_pci_ids);

/* pci_driver definition */
static struct pci_driver snd_sof_pci_amd_acp63_driver = {
	.name = KBUILD_MODNAME,
	.id_table = acp63_pci_ids,
	.probe = acp63_pci_probe,
	.remove = acp63_pci_remove,
	.driver = {
		.pm = pm_ptr(&sof_pci_pm),
	},
};
module_pci_driver(snd_sof_pci_amd_acp63_driver);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("ACP63 SOF Driver");
MODULE_IMPORT_NS("SND_SOC_SOF_AMD_COMMON");
MODULE_IMPORT_NS("SND_SOC_SOF_PCI_DEV");
