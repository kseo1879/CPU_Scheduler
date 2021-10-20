/* SPDX-License-Identifier: GPL-2.0 */



#ifdef CONFIG_BLOCK

struct writeback_control;
struct readahead_control;

void mpage_readahead(struct readahead_control *, get_block_t get_block);
int mpage_readpage(struct page *page, get_block_t get_block);
int mpage_writepages(struct address_space *mapping,
		struct writeback_control *wbc, get_block_t get_block);
int mpage_writepage(struct page *page, get_block_t *get_block,
		struct writeback_control *wbc);

#endif
