include mk/subdir_pre.mk

bin_mem_pic := $(d)mem_pic

OBJS := $(d)mem_pic.o

$(bin_mem_pic): $(OBJS) $(lib_pic) $(lib_mem) $(lib_base) $(lib_address_translation)


CLEAN := $(bin_mem_pic) $(OBJS)
ALL := $(bin_mem_pic)

include mk/subdir_post.mk
