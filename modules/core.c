int memcpy_sp(void *dst, size_t dst_size, const void *src, size_t src_len)
{
    /* 1. Reject NULL pointers */
    if (!dst || !src)
        return -EINVAL;

    /* 2. Reject zero-sized destination (nothing can fit) */
    if (dst_size == 0)
        return -ENOSPC;

    /* 3. Overflow-safe bounds check: src_len must fit in dst_size */
    if (src_len > dst_size)
        return -ENOSPC;

    /* 4. Perform the copy only after validation */
    memcpy(dst, src, src_len);
    return 0;
}
