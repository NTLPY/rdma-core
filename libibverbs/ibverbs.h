/*
 * Copyright (c) 2004, 2005 Topspin Communications.  All rights reserved.
 * Copyright (c) 2007 Cisco Systems, Inc.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef IB_VERBS_H
#define IB_VERBS_H

#include <pthread.h>

#include <infiniband/driver.h>
#include <util/bitmap.h>

#define INIT		__attribute__((constructor))

#define PFX		"libibverbs: "
#define VERBS_OPS_NUM (sizeof(struct verbs_context_ops) / sizeof(void *))

struct ibv_abi_compat_v2 {
	struct ibv_comp_channel	channel;
	pthread_mutex_t		in_use;
};

extern int abi_ver;
extern const struct verbs_context_ops verbs_dummy_ops;

/**
 * @brief Get the list of ibverbs devices.
 *
 * This function updates the provided list with the available InfiniBand devices.
 *
 * @param[inout] list A pointer to a list_head structure that will be
 * updated with the available InfiniBand devices verbs_device.
 * @return The number of devices found, or a negative error code on failure.
 */
int ibverbs_get_device_list(struct list_head *list);
/**
 * Initialize the ibverbs library.
 *
 * This function is called automatically by ibv_get_device_list().
 */
int ibverbs_init(void);
/**
 * Decrement the reference count of the underlying verbs_device and release it when it reaches zero.
 */
void ibverbs_device_put(struct ibv_device *dev);
/**
 * Increment the reference count of the underlying verbs_device.
 */
void ibverbs_device_hold(struct ibv_device *dev);
int __lib_query_port(struct ibv_context *context, uint8_t port_num,
		     struct ibv_port_attr *port_attr, size_t port_attr_len);
/**
 * Setup verbs_sysfs_dev from uverbs device path.
 *
 * @param[in] uv_dirfd File descriptor of the uverbs device directory.
 * @param[in] uverbs Name of uverbs device.
 * @param[out] sysfs_dev Pointer to the sysfs device structure to be filled.
 *
 * Following fields will be populated:
 * 1. Name of uverbs device
 * 2. Modified time of InfiniBand device
 * 3. Device number
 * 4. ABI version
 *
 * @return 0 on success, or error code on failure.
 */
int setup_sysfs_uverbs(int uv_dirfd, const char *uverbs,
		       struct verbs_sysfs_dev *sysfs_dev);

#ifdef _STATIC_LIBRARY_BUILD_
static inline void load_drivers(void)
{
}
#else
/**
 * @brief Load the drivers from the configuration directory and environment variables.
 *
 * Load drivers from the configuration directory and environment variables `RDMAV_DRIVERS` and `IBV_DRIVERS`.
 */
void load_drivers(void);
#endif

struct verbs_ex_private {
	BMP_DECLARE(unsupported_ioctls, VERBS_OPS_NUM);
	uint32_t driver_id;
	bool use_ioctl_write;
	struct verbs_context_ops ops;
	bool imported;
};

/**
 * Dynamic cast ibv_context to verbs_ex_private.
 */
static inline struct verbs_ex_private *get_priv(struct ibv_context *ctx)
{
	return container_of(ctx, struct verbs_context, context)->priv;
}

/**
 * Get the verbs context operations for a given context.
 */
static inline const struct verbs_context_ops *get_ops(struct ibv_context *ctx)
{
	return &get_priv(ctx)->ops;
}

/**
 * Decode kernel node type.
 * @param[in] knode_type Kernel node type to decode.
 * @return Decoded InfiniBand node type.
 */
enum ibv_node_type decode_knode_type(unsigned int knode_type);

int find_sysfs_devs_nl(struct list_head *tmp_sysfs_dev_list);

/**
 * Try to access the char verbs device.
 * @param[in] sysfs_dev Pointer to the verbs_sysfs_dev structure.
 * @return 0 on success, or error code on failure.
 * @see RDMA_CDEV_DIR
 */
int try_access_device(const struct verbs_sysfs_dev *sysfs_dev);

#endif /* IB_VERBS_H */
