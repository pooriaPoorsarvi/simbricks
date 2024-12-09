include mk/subdir_pre.mk

lib_pic := $(d)libpic.a

OBJS := $(addprefix $(d),if.o)

libsimbricks_objs += $(OBJS)

$(lib_pic): $(OBJS)

CLEAN := $(lib_pic) $(OBJS)
include mk/subdir_post.mk
