/*
 * Copyright (c) 2016-2021, NVIDIA CORPORATION. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PVA_TASK_H
#define PVA_TASK_H

#include <pva-bit.h>
#include <pva-types.h>
#include <pva-packed.h>
#include <pva-sys-dma.h>

#define TASK_VERSION_ID 0x01U
#define PVA_TASK_VERSION_ID 0x01U
#define PVA_ENGINE_ID 'P'

#define PVA_MAX_PREACTION_LISTS 1U
#define PVA_MAX_POSTACTION_LISTS 1U

#define PVA_TASK_POINTER_AUX_SIZE_MASK 0x00ffffffffffffffU
#define PVA_TASK_POINTER_AUX_SIZE_SHIFT 0
#define PVA_TASK_POINTER_AUX_FLAGS_MASK 0xff00000000000000U
#define PVA_TASK_POINTER_AUX_FLAGS_SHIFT 56
#define PVA_TASK_POINTER_AUX_FLAGS_CVNAS (1U << 0)

#define NVPVA_TENSOR_MAX_DIMENSIONS (9u)

#define NVPVA_TENSOR_ATTR_DIMENSION_ORDER_NHWC 0x00000001U
#define NVPVA_TENSOR_ATTR_DIMENSION_ORDER_NCHW 0x00000002U
#define NVPVA_TENSOR_ATTR_DIMENSION_ORDER_NCxHWx 0x00000003U
#define NVPVA_TENSOR_ATTR_DIMENSION_ORDER_NDHWC 0x00000004U
#define NVPVA_TENSOR_ATTR_DIMENSION_ORDER_NCDHW 0x00000005U
#define NVPVA_TENSOR_ATTR_DIMENSION_ORDER_IMPLICIT 0x00000006U

/*
 * Generic task meta-data for the CV pipeline.
 */
typedef uint16_t pva_task_ofs;

struct PVA_PACKED pva_gen_task_s {
	pva_iova next; /* ptr to next task in the list */
	uint8_t versionid;
	uint8_t engineid;
	pva_task_ofs length;
	uint16_t sequence;
	uint8_t n_preaction_lists;
	uint8_t n_postaction_lists;
	pva_task_ofs preaction_lists_p;
	pva_task_ofs postaction_lists_p;
};

/*
 * Structure pointed to by {pre/post}action_lists_p.  This points
 * to the actual action list.
 */
struct PVA_PACKED pva_action_list_s {
	pva_task_ofs offset;
	uint16_t length;
};

/** @defgroup TASK_ACT PVA Task Action Identifiers.
 *
 * @{
 */
#define TASK_ACT_TERMINATE 0x00U
#define TASK_ACT_PVA_STATISTICS 0x40U
#define TASK_ACT_PTR_BLK_GTREQL 0x92U
#define TASK_ACT_READ_STATUS 0xC0U
#define TASK_ACT_WRITE_STATUS 0xC1U
#define TASK_ACT_PTR_WRITE_SOT_V 0xC2U
#define TASK_ACT_PTR_WRITE_SOT_R 0xC3U
#define TASK_ACT_PTR_WRITE_EOT_V 0xC4U
#define TASK_ACT_PTR_WRITE_EOT_R 0xC5U
#define TASK_ACT_PTR_WRITE_EOT 0xC6U
/** @} */

struct PVA_PACKED pva_gen_task_status_s {
	uint64_t timestamp;
	uint32_t engine_status;
	uint16_t subframe;
	uint16_t status_task;
};

struct PVA_PACKED pva_task_statistics_s {
	uint64_t queued_time; /* when task was accepted by PVA */
	uint64_t head_time; /* when task reached head of queue */
	uint64_t input_actions_complete; /* when input actions done */
	uint64_t vpu_assigned_time; /* when task assigned a VPU */
	uint64_t vpu_start_time; /* when VPU started running task */
	uint64_t vpu_complete_time; /* when execution completed */
	uint64_t complete_time; /* when task considered complete */
	uint8_t vpu_assigned; /* which VPU task was assigned */
	uint8_t reserved[7];
};

/** @defgroup PVA_TASK_FL Flags in PVA runlist */
/** @{ */
#define PVA_TASK_FL_ATOMIC PVA_BIT(0U)
#define PVA_TASK_FL_CV_RD_SCALARS PVA_BIT(1U)
#define PVA_TASK_FL_CV_WR_SCALARS PVA_BIT(2U)
#define PVA_TASK_FL_CV_WR_ROI PVA_BIT(3U)
#define PVA_TASK_FL_VPU_DEBUG PVA_BIT(4U)
#define PVA_TASK_FL_VPU_CRASH_DUMP PVA_BIT(5U)
/** @} */

enum pva_task_parameter_type_e {
	PVA_PARAM_FIRST = 0U, /* must match first type */
	PVA_PARAM_SCALAR_LIST = 0U,
	PVA_PARAM_SURFACE_LIST = 1U,
	PVA_PARAM_ROI_LIST = 2U,
	PVA_PARAM_2DPOINTS_LIST = 3U,
	PVA_PARAM_OPAQUE_DATA = 4U,
	PVA_PARAM_LAST = 5U /* must be last! */
};

struct PVA_PACKED pva_task_opaque_data_desc_s {
	/* Number of bytes in the primary payload */
	uint16_t primary_payload_size;
};

struct PVA_PACKED pva_task_pointer_s {
	uint64_t address;
	uint64_t aux;
};

struct PVA_PACKED pva_task_parameter_array_s {
	pva_iova address;
	uint32_t size;
	uint32_t type; /* type = pva_task_parameter_type_e */
};

/*
 * Parameter descriptor (all parameters have the same header)
 * the specific data for the parameters immediately follows
 * the descriptor.
 */
struct PVA_PACKED pva_task_parameter_desc_s {
	uint32_t num_parameters;
	uint32_t reserved;
};

/*
 * Individual Region of Interest (ROI) descriptor
 */
struct PVA_PACKED pva_task_roi_desc_s {
	uint32_t left;
	uint32_t top;
	uint32_t right;
	uint32_t bottom;
};

/*
 * Surface descriptor
 */
struct PVA_PACKED pva_task_surface_s {
	pva_iova address;
	pva_iova roi_addr;
	uint32_t roi_size;
	uint32_t surface_size;
	uint32_t width;
	uint32_t height;
	uint32_t line_stride;
	uint32_t plane_stride;
	uint32_t num_planes;
	uint8_t layout;
	uint8_t block_height_log2;
	uint8_t memory;
	uint8_t reserved;
	uint64_t format;
};

/*
 * 2-dimensional point descriptor
 */
struct PVA_PACKED pva_task_point2d_s {
	uint32_t x;
	uint32_t y;
};

/*
 * Surface Layout.
 */
#define PVA_TASK_SURFACE_LAYOUT_PITCH_LINEAR 0U
#define PVA_TASK_SURFACE_LAYOUT_BLOCK_LINEAR 1U

/*
 * Where the surface is located.
 */
#define PVA_TASK_SURFACE_MEM_FL_CV_SURFACE PVA_BIT(0U)
#define PVA_TASK_SURFACE_MEM_FL_CV_ROI PVA_BIT(1U)

/**
 * @brief Task descriptor for the new architecture.
 *
 * The runlist of the new task descriptor contains pointer to
 * task-specific parameters of the VPU app, pointer to info structure
 * describing its binary code, and its dma setup.
 */
struct PVA_PACKED pva_td_s {
	struct pva_gen_task_s gen_task;
	uint8_t runlist_version;
	uint8_t pva_td_pad0[7];
	uint16_t flags;
	/** Number of parameters in parameter_base array */
	uint16_t num_parameters;
	/** IOVA pointer to an array of struct pva_vpu_parameter_s */
	pva_iova parameter_base;
	/** IOVA pointer to a struct pva_bin_info_s structure */
	pva_iova bin_info;
	/** IOVA pointer to a struct pva_dma_info_s structure */
	pva_iova dma_info;
	uint64_t timeout;
	/** Size of L2SRAM required for the task */
	uint32_t l2sram_size;
	/** Index of the stream ID assigned to this task */
	uint32_t sid_index;
	uint8_t r5_reserved[32] __aligned(8);
};

/** Runlist version for new task descriptor format */
#define PVA_RUNLIST_VERSION_ID (0x02U)

/** @addtogroup PVA_TASK_FL
 * @{
 */
/** Schedule on VPU0 only */
#define PVA_TASK_FL_VPU0 PVA_BIT(8U)

/** Schedule on VPU1 only */
#define PVA_TASK_FL_VPU1 PVA_BIT(9U)

/** Schedule next task in list immediately on this VPU.
 *
 * Not allowed in the last task of batch list.
 */
#define PVA_TASK_FL_HOT_VPU PVA_BIT(10U)

/** Pass L2RAM allocation to the next task in list.
 *
 * Not allowed in the last task of batch list.
 */
#define PVA_TASK_FL_KEEP_L2RAM PVA_BIT(11U)

/** Flag to identify a barrier task */
#define PVA_TASK_FL_SYNC_TASKS PVA_BIT(12U)

/** Flag to indicate timer start */
#define PVA_TASK_FL_TIMER_START PVA_BIT(13U)

/** Flag to indicate timer stop */
#define PVA_TASK_FL_TIMER_STOP PVA_BIT(14U)
/** @} */

/** Version of the binary info */
#define PVA_BIN_INFO_VERSION_ID (0x01U)
#define PVA_MAX_VPU_DATA_SECTION (4U)
#define PVA_MAX_VPU_METADATA (4U)

#define PVA_CODE_SEC_BASE_ADDR_ALIGN (128ULL)
#define PVA_CODE_SEC_SIZE_ALIGN (32U)

#define PVA_DATA_SEC_BASE_ADDR_ALIGN (64ULL)
#define PVA_DATA_SEC_SIZE_ALIGN (32U)

struct pva_vpu_data_section_s {
	uint32_t offset; /**< Offset from the base source address */
	uint32_t addr; /**< Target address (VMEM offset) */
	uint32_t size; /**< Size of the section in bytes */
};

/** @brief Information of a VPU app binary.
 *
 * The PVA kernels are implemented as VPU apps, small VPU programs
 * executed independently on a VPU. The information structure is used
 * by PVA R5 to preload the code in the VPU icache as well as preload
 * the data sections into the VPU VMEM.
 *
 * If PVA has multiple address spaces, the application code, data, and
 * metadata may be placed in different address space domains accessed
 * using different StreamIDs. The code is accessed by VPU, the data
 * sections by PVA DMA, the metadata by R5.
 *
 * The metadata sections contain the ABI information of the VPU
 * app. The metadata is stored as data sections in the ELF executable,
 * however, the address of the metadata section is >= 768K (0xC0000).
 */
struct PVA_PACKED pva_bin_info_s {
	uint16_t bin_info_size; /**< Size of this structure */
	uint16_t bin_info_version; /**< PVA_BIN_INFO_VERSION_ID */

	/** Size of the code */
	uint32_t code_size;
	/** Base address of the code. Should be aligned at 128.  */
	pva_iova code_base;

	/** Data sections */
	struct pva_vpu_data_section_s data[PVA_MAX_VPU_DATA_SECTION];
	/** Base address of the data. Should be aligned at 64. */
	pva_iova data_base;

	/** Metadata sections */
	struct pva_vpu_data_section_s metadata[PVA_MAX_VPU_METADATA];
	/** Base address of the metadata. Recommended alignment is 16. */
	pva_iova meta_base;
};

/*
 * Status structure that will be return to circular buffer
 */
struct PVA_PACKED pva_task_error_s {
	/* IOVA address of task */
	pva_iova addr;

	/* Status of task execution */
	uint16_t error;

	/* Indicates if status is valid */
	uint8_t valid;

	/* VPU id on which the task was scheduled */
	uint8_t vpu;

	/* Queue to which the task belongs */
	uint8_t queue;
};
#endif
