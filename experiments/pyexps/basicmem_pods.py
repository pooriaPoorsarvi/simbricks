
import simbricks.orchestration.experiments as exp
import simbricks.orchestration.nodeconfig as nodec
import simbricks.orchestration.simulators as sim
from simbricks.orchestration.simulators import QemuCPUNodeHost

experiments = []


class MemTest(nodec.AppConfig):

    def __init__(self, mem: sim.BasicMemDev):
        self.start_address = mem.addr/(1024*1024*1024)

    def run_cmds(self, node):
        return [
            f'dd if=/dev/mem bs=1G skip={int(self.start_address)} count=1 status=progress | xxd | less',
        ]



node_config = nodec.NodeConfig()
node_config.should_accelerate = False
node_config.far_memory_size = 1024 * 1024 * 1024
node_config.memory = 12 * 1024

print("sat far memory size to ", node_config.far_memory_size)

e = exp.Experiment('basicmem-' + 'pods')
e.checkpoint = False

mem = sim.BasicMemDev()
mem.name = 'mem0'
mem.addr = (node_config.memory * 1024 * 1024) - node_config.far_memory_size
mem.size = node_config.far_memory_size

node_config.nockp = True
node_config.app = MemTest(mem)

host = QemuCPUNodeHost(node_config)

host.name = 'host.0'

e.add_memdev(mem)

e.add_host(host)

host.wait = True


host.add_memdev(mem)

experiments.append(e)
