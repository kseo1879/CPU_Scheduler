


#ifndef TTM_SET_MEMORY
#define TTM_SET_MEMORY

#include <linux/mm.h>

#ifdef CONFIG_X86

#include <asm/set_memory.h>

static inline int ttm_set_pages_array_wb(struct page **pages, int addrinarray)
{
	return set_pages_array_wb(pages, addrinarray);
}

static inline int ttm_set_pages_array_wc(struct page **pages, int addrinarray)
{
	return set_pages_array_wc(pages, addrinarray);
}

static inline int ttm_set_pages_array_uc(struct page **pages, int addrinarray)
{
	return set_pages_array_uc(pages, addrinarray);
}

static inline int ttm_set_pages_wb(struct page *page, int numpages)
{
	return set_pages_wb(page, numpages);
}

static inline int ttm_set_pages_wc(struct page *page, int numpages)
{
	unsigned long addr = (unsigned long)page_address(page);

	return set_memory_wc(addr, numpages);
}

static inline int ttm_set_pages_uc(struct page *page, int numpages)
{
	return set_pages_uc(page, numpages);
}

#else 

#if IS_ENABLED(CONFIG_AGP)

#include <asm/agp.h>

static inline int ttm_set_pages_array_wb(struct page **pages, int addrinarray)
{
	int i;

	for (i = 0; i < addrinarray; i++)
		unmap_page_from_agp(pages[i]);
	return 0;
}

static inline int ttm_set_pages_array_wc(struct page **pages, int addrinarray)
{
	int i;

	for (i = 0; i < addrinarray; i++)
		map_page_into_agp(pages[i]);
	return 0;
}

static inline int ttm_set_pages_array_uc(struct page **pages, int addrinarray)
{
	int i;

	for (i = 0; i < addrinarray; i++)
		map_page_into_agp(pages[i]);
	return 0;
}

static inline int ttm_set_pages_wb(struct page *page, int numpages)
{
	int i;

	for (i = 0; i < numpages; i++)
		unmap_page_from_agp(page++);
	return 0;
}

#else 

static inline int ttm_set_pages_array_wb(struct page **pages, int addrinarray)
{
	return 0;
}

static inline int ttm_set_pages_array_wc(struct page **pages, int addrinarray)
{
	return 0;
}

static inline int ttm_set_pages_array_uc(struct page **pages, int addrinarray)
{
	return 0;
}

static inline int ttm_set_pages_wb(struct page *page, int numpages)
{
	return 0;
}

#endif 

static inline int ttm_set_pages_wc(struct page *page, int numpages)
{
	return 0;
}

static inline int ttm_set_pages_uc(struct page *page, int numpages)
{
	return 0;
}

#endif 

#endif
