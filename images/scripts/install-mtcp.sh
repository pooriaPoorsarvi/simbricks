#!/bin/bash -eux

set -eux

apt-get -y install libnuma-dev libgmp-dev bc python

cd /tmp
wget https://ftp.gnu.org/gnu/automake/automake-1.16.2.tar.gz
tar xf automake-1.16.2.tar.gz
cd automake-1.16.2
./configure
make -j4 install

cd /tmp
git clone https://github.com/mtcp-stack/mtcp.git /root/mtcp
cd /root/mtcp
git submodule init
git submodule update

export RTE_SDK=$PWD/dpdk
export RTE_TARGET=x86_64-native-linuxapp-gcc

sed -i -e 's/O_TO_EXE_STR =/\$(shell if [ \! -d \${RTE_SDK}\/\${RTE_TARGET}\/lib ]\; then mkdir \${RTE_SDK}\/\${RTE_TARGET}\/lib\; fi)\nLINKER_FLAGS = \$(call linkerprefix,\$(LDLIBS))\n\$(shell echo \${LINKER_FLAGS} \> \${RTE_SDK}\/\${RTE_TARGET}\/lib\/ldflags\.txt)\nO_TO_EXE_STR =/g' $RTE_SDK/mk/rte.app.mk

sed -i -e 's/-Werror//' dpdk/kernel/linux/kni/Makefile
sed -i -e 's/-Werror//' dpdk/kernel/linux/igb_uio/Makefile
sed -i -e 's/-Werror//' dpdk/mk/toolchain/gcc/rte.vars.mk
rm -rf dpdk/.git
sed -i \
    -e 's/CONFIG_RTE_LIBRTE_I40E_DEBUG_RX=n/CONFIG_RTE_LIBRTE_I40E_DEBUG_RX=y/' \
    -e 's/CONFIG_RTE_LIBRTE_I40E_DEBUG_TX=n/CONFIG_RTE_LIBRTE_I40E_DEBUG_TX=y/' \
    -e 's/CONFIG_RTE_LIBRTE_I40E_DEBUG_TX_FREE=n/CONFIG_RTE_LIBRTE_I40E_DEBUG_TX_FREE=y/' \
    -e 's/CONFIG_RTE_LOG_LEVEL=RTE_LOG_INFO/CONFIG_RTE_LOG_LEVEL=RTE_LOG_DEBUG/' \
    dpdk/config/common_base
make -j4 -C dpdk install T=$RTE_TARGET

./configure --with-dpdk-lib=$RTE_SDK/$RTE_TARGET CFLAGS="-DMAX_CPUS=8"

sed -i -e 's://#define DEBUG:#define DEBUG:' \
    -e 's/RTE_LOG_EMERG/RTE_LOG_DEBUG/g' \
    dpdk-iface-kmod/dpdk_iface_main.c
(cd dpdk-iface-kmod && make -j4)
make -j4

git clone https://github.com/tcp-acceleration-service/tas.git /root/tas
(cd /root/tas && make lib/utils/rng.o)

git clone https://github.com/FreakyPenguin/benchmarks.git /root/tasbench
cd /root/tasbench/micro_rpc
make echoserver_mtcp testclient_mtcp MTCP_BASE="/root/mtcp" TAS_CODE="/root/tas"

echo "blacklist i40e" > /etc/modprobe.d/i40e_bl.conf