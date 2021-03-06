#include "nic/utils/block_list/block_list.hpp"
#include "nic/include/hal_mem.hpp"

namespace hal {
namespace utils {

//-----------------------------------------------------------------------------
// Factory method to construct the block list
//-----------------------------------------------------------------------------
block_list *
block_list::factory (uint32_t elem_size, uint32_t elems_per_block, shmmgr *mmgr)
{
    block_list  *new_block_list = NULL;
    void        *mem;

    if (elems_per_block == 0 || elem_size == 0) {
        return NULL;
    }

    if (mmgr) {
        mem = mmgr->alloc(sizeof(block_list), 4, true);
    } else {
        mem = HAL_CALLOC(HAL_MEM_ALLOC_BLOCK_LIST, sizeof(block_list));
    }

    if (mem == NULL) {
        return NULL;
    }

    new_block_list = new (mem) block_list();
    if (new_block_list->init(elem_size, elems_per_block, mmgr) == false) {
        new_block_list->~block_list();
        if (mmgr) {
            mmgr->free(mem);
        } else {
            HAL_FREE(HAL_MEM_ALLOC_BLOCK_LIST, mem);
        }
        return NULL;
    }

    return new_block_list;
}

//-----------------------------------------------------------------------------
// Init method to construct the block list
//-----------------------------------------------------------------------------
bool
block_list::init(uint32_t elem_size, uint32_t elems_per_block, shmmgr *mmgr)
{
    mmgr_                  = mmgr;
    this->elem_size_       = elem_size;
    this->elems_per_block_ = elems_per_block;
    dllist_reset(&this->list_head_);

    return true;
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
block_list::~block_list()
{
    dllist_ctxt_t    *curr, *next;
    list_node_t      *node = NULL;

    dllist_for_each_safe(curr, next, &list_head_) {
        node = dllist_entry(curr, list_node_t, ctxt_);
        dllist_del(&node->ctxt_);
        if (mmgr_) {
            mmgr_->free(node);
        } else {
            HAL_FREE(HAL_MEM_ALLOC_BLOCK_LIST_NODE, node);
        }
    }
}

//-----------------------------------------------------------------------------
// Destroy method to free back the block list instance
//-----------------------------------------------------------------------------
void
block_list::destroy(block_list *blist)
{
    shmmgr    *mmgr;

    if (!blist) {
        return;
    }
    mmgr = blist->mmgr_;
    blist->~block_list();
    if (mmgr) {
        mmgr->free(blist);
    } else {
        HAL_FREE(HAL_MEM_ALLOC_BLOCK_LIST, blist);
    }
}

//-----------------------------------------------------------------------------
// Remove all blocks
//-----------------------------------------------------------------------------
hal_ret_t 
block_list::remove_all()
{
    // make sure the destructor is just removing blocks
    this->~block_list();

    return HAL_RET_OK;
}

//-----------------------------------------------------------------------------
// Gets last node in the list of nodes
//-----------------------------------------------------------------------------
list_node_t *
block_list::get_last_node_(void)
{
    dllist_ctxt_t               *curr, *next;
    list_node_t                 *last_node = NULL;

    dllist_for_each_safe(curr, next, &list_head_) {
        last_node = dllist_entry(curr, list_node_t, ctxt_);
    }

    return last_node;
}

//-----------------------------------------------------------------------------
// Insert an element
//-----------------------------------------------------------------------------
hal_ret_t
block_list::insert(void *elem)
{
    hal_ret_t       ret = HAL_RET_OK;
    list_node_t     *last_node = NULL, *insert_in_node = NULL;
    void            *loc = NULL;

    if (this->is_present(elem)) {
        // element already present
        ret = HAL_RET_ENTRY_EXISTS;
        goto end;
    }


    last_node = this->get_last_node_();

    // If first block or no space in last block
    if (last_node == NULL || last_node->num_in_use_ == elems_per_block_) {
        // Allocate new block
        if (mmgr_) {
            insert_in_node =
                (list_node_t *)mmgr_->alloc(sizeof(list_node_t) +
                                            (elems_per_block_ * elem_size_),
                                            4, false);
        } else {
            insert_in_node =
                (list_node_t *)HAL_MALLOC(HAL_MEM_ALLOC_BLOCK_LIST_NODE, 
                                          sizeof(list_node_t) +
                                          (elems_per_block_ * elem_size_));
        }
        if (insert_in_node == NULL) {
            HAL_TRACE_DEBUG("{}:Unable to allocate memory for list node",
                            __FUNCTION__);
            ret = HAL_RET_OOB;
            goto end;
        }

        insert_in_node->num_in_use_ = 0;
        dllist_reset(&insert_in_node->ctxt_);

        // Insert the block
        dllist_add_tail(&list_head_, &insert_in_node->ctxt_);
    } else {
        insert_in_node = last_node;
    }

    // Insert the element in the block
    loc = element_location_(insert_in_node, insert_in_node->num_in_use_);
    memcpy(loc, elem, this->elem_size_);
    insert_in_node->num_in_use_++;

end:

    return ret;
}


//-----------------------------------------------------------------------------
// Check for the presence of element
//-----------------------------------------------------------------------------
bool
block_list::is_present(void *elem)
{
    list_node_t         *node = NULL;
    uint32_t            elem_id = 0;
    hal_ret_t           ret = HAL_RET_OK;

    ret = this->find_(elem, &node, &elem_id);
    if (ret == HAL_RET_OK) {
        // Match
        return true;
    }
    return false;
}


//-----------------------------------------------------------------------------
// Find an element
//-----------------------------------------------------------------------------
hal_ret_t
block_list::find_(void *elem, list_node_t **elem_in_node, 
                 uint32_t *elem_id)
{
    hal_ret_t           ret = HAL_RET_ENTRY_NOT_FOUND;
    dllist_ctxt_t       *curr, *next;
    list_node_t         *node = NULL;
    void                *loc = NULL;

    *elem_in_node = NULL;
    *elem_id = 0;

    dllist_for_each_safe(curr, next, &list_head_) {
        node = dllist_entry(curr, list_node_t, ctxt_);
        for (int i = 0; i < node->num_in_use_; i++) {
            loc = element_location_(node, i);
            if (!memcmp(loc, elem, this->elem_size_)) {
                // Match
                *elem_in_node = node;
                *elem_id = i;
                ret = HAL_RET_OK;
                goto end;
            }
        }
    }

end:

    return ret;
}

//-----------------------------------------------------------------------------
// Get element location within a node
//-----------------------------------------------------------------------------
void *
block_list::element_location_(list_node_t *node, uint32_t elem_id)
{
    return (((uint8_t *)node) + sizeof(list_node_t) +
            elem_id * this->elem_size_);
}

//-----------------------------------------------------------------------------
// Consolidate the block list
//-----------------------------------------------------------------------------
hal_ret_t
block_list::consolidate_(list_node_t *node, uint32_t elem_id,
                         list_node_t *last_node)
{
    hal_ret_t           ret = HAL_RET_OK;
    void                *loc = NULL, *last_elem_loc = NULL;

    if (node == NULL || last_node == NULL) {
        return HAL_RET_INVALID_ARG;
    }

    // Copy last element into removed location
    loc = element_location_(node, elem_id);
    last_elem_loc = element_location_(last_node, last_node->num_in_use_ - 1);
    memcpy(loc, last_elem_loc, this->elem_size_);

    // Remove last location
    last_node->num_in_use_--;
    if (!last_node->num_in_use_) {
        // Last element of Last node
        // Delete & free last node
        dllist_del(&last_node->ctxt_);
        if (mmgr_) {
            mmgr_->free(last_node);
        } else {
            HAL_FREE(HAL_MEM_ALLOC_BLOCK_LIST_NODE, last_node);
        }
    }

    return ret;
}

//-----------------------------------------------------------------------------
// Remove an element
//-----------------------------------------------------------------------------
hal_ret_t
block_list::remove(void *elem)
{
    hal_ret_t           ret = HAL_RET_OK;
    list_node_t         *node = NULL, *last_node = NULL;
    uint32_t            elem_id = 0;


    ret = this->find_(elem, &node, &elem_id);
    if (ret == HAL_RET_ENTRY_NOT_FOUND) {
        HAL_TRACE_DEBUG("{}:Entry not found", __FUNCTION__);
        goto end;
    }

    last_node = this->get_last_node_();
    if (node == last_node && node->num_in_use_ == (elem_id + 1)) {
        // Last element, no need to consolidate
        node->num_in_use_--;

        if (!node->num_in_use_) {
            // Last element of Last node
            // Delete & free last node
            dllist_del(&node->ctxt_);
            if (mmgr_) {
                mmgr_->free(node);
            } else {
                HAL_FREE(HAL_MEM_ALLOC_BLOCK_LIST_NODE, node);
            }
        }
    } else {
        this->consolidate_(node, elem_id, last_node);
    }

end:

    return ret;
}

//-----------------------------------------------------------------------------
// Get number of elements 
//-----------------------------------------------------------------------------
uint32_t
block_list::num_elems(void)
{
    dllist_ctxt_t       *curr, *next;
    list_node_t         *node = NULL;
    uint32_t            count = 0;

    dllist_for_each_safe(curr, next, &list_head_) {
        node = dllist_entry(curr, list_node_t, ctxt_);
        count += node->num_in_use_;
    }

    return count;
}

hal_ret_t
block_list::remove_elem_(list_node_t *node, uint32_t elem_id, bool *last_elem) 
{
    list_node_t         *last_node = NULL;

    *last_elem = false;

    last_node = this->get_last_node_();
    if (node == last_node && node->num_in_use_ == (elem_id + 1)) {
        // Last element, no need to consolidate
        node->num_in_use_--;
        *last_elem = true;

        if (!node->num_in_use_) {
            // Last element of Last node
            // Delete & free last node
            dllist_del(&node->ctxt_);
            if (mmgr_) {
                mmgr_->free(node);
            } else {
                HAL_FREE(HAL_MEM_ALLOC_BLOCK_LIST_NODE, node);
            }
        }
    } else {
        this->consolidate_(node, elem_id, last_node);
    }

    return HAL_RET_OK;
}

//-----------------------------------------------------------------------------
// Iterate the elements
//-----------------------------------------------------------------------------
hal_ret_t 
block_list::iterate(block_list_cb_t cb, void *data)
{
    hal_ret_t       ret           = HAL_RET_OK;
    dllist_ctxt_t   *curr, *next;
    list_node_t     *node         = NULL;
    void            *loc          = NULL;
    bool            rv            = true;

    dllist_for_each_safe(curr, next, &list_head_) {
        node = dllist_entry(curr, list_node_t, ctxt_);
        for (int i = 0; i<node->num_in_use_; i++) {
            loc = element_location_(node, i);
            rv = cb(loc, data);
            if (!rv) {
                goto end;
            }
        }
    }

end:

    return ret;
}

block_list& 
block_list::operator+=(const block_list& rhs) 
{
    dllist_ctxt_t   *curr, *next;
    list_node_t     *node         = NULL;
    void            *loc          = NULL;

    dllist_for_each_safe(curr, next, &list_head_) {
        node = dllist_entry(curr, list_node_t, ctxt_);
        for (int i = 0; i<node->num_in_use_; i++) {
            loc = element_location_(node, i);
            this->insert(loc);
        }
    }

    return *this;
}


} // namespace utils
} // namespace hal
