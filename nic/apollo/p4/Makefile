TOPDIR = ../..

SUBDIRS = p4 \
          txdma \
          rxdma \
          nvme \
          $(NULL)

.PHONY: subdirs $(SUBDIRS)
default: subdirs

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

include $(TOPDIR)/make/Makefile.inc
