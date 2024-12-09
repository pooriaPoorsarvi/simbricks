include mk/subdir_pre.mk

bin_cpu_pic := $(d)cpu_pic

OBJS := $(d)cpu_pic.o

$(bin_cpu_pic): $(OBJS) $(lib_pic) $(lib_mem) $(lib_base) $(lib_address_translation)

CLEAN := $(bin_cpu_pic) $(OBJS)
ALL := $(bin_cpu_pic)

include mk/subdir_post.mk
