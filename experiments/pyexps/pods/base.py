
import simbricks.orchestration.experiments as exp
import simbricks.orchestration.nodeconfig as nodec
import simbricks.orchestration.simulators as sim
from simbricks.orchestration.simulators import QemuCPUNodeHost, CPUPIC, MemPIC



class MemTest(nodec.AppConfig):

    def __init__(self, 
                 start_address: int,
                 is_read: bool = True,
                ):
        self.start_address = start_address
        self.is_read = is_read

    def run_cmds(self, node):
        main_command = ''
        if self.is_read:
            main_command = f'dd if=/dev/mem bs=1G skip={int(self.start_address)} count=1 status=progress | xxd | less'
        else:
            main_command = f'dd if=/dev/zero of=/dev/mem bs=1G seek={int(self.start_address)} count=1 status=progress'
        return [
            f'START=$(date +%s%3N)',
            main_command,
            f'END=$(date +%s%3N)',
            f'echo "Elapsed time: $((END - START)) ms"',
        ]


def setup_pics(
    e: exp.Experiment,
    node_config: nodec.NodeConfig,
    mem_device: sim.BasicMemDev,
    host: QemuCPUNodeHost,
):
    mem_pic = MemPIC(node_config, 'mem0_pic0')
    mem_pic.set_mem_dev(mem_device)


    cpu_pic = CPUPIC(node_config, 'cpu0_pic0')
    cpu_pic.set_mem_pic(mem_pic)

    e.add_memdev(mem_pic)
    e.add_memdev(cpu_pic)
    
    host.add_memdev(cpu_pic)

def setup_far_memory_without_pics(
    e: exp.Experiment,
    node_config: nodec.NodeConfig,
    mem_device: sim.BasicMemDev,
    host: QemuCPUNodeHost,
):
    host.add_memdev(mem_device)
    


def create_expermient(
    experiment_name: str,
    use_pics: bool = True,
    use_far_off_memory: bool = True,
    start_address_gb: int = 11,
    is_read: bool = True,
) -> exp.Experiment:

    # Create node
    node_config = nodec.NodeConfig()
    node_config.should_accelerate = False
    node_config.memory = 12 * 1024
    node_config.nockp = True
    node_config.app = MemTest(
        start_address=start_address_gb,
        is_read=is_read,
    )

    # Create experiment
    e = exp.Experiment(experiment_name)
    e.checkpoint = False

    # Create host
    host = QemuCPUNodeHost(node_config)
    host.name = 'host.0.qemu'
    host.wait = True
    
    # Add host to experiment
    e.add_host(host)
    


    if use_far_off_memory:
        # Set far memory size
        node_config.far_memory_size = 1024 * 1024 * 1024
        print("sat far memory size to ", node_config.far_memory_size)
        
        # Create memory device for far memory
        mem_device = sim.BasicMemDev()
        mem_device.name = 'mem0'
        mem_device.addr = (node_config.memory * 1024 * 1024) - node_config.far_memory_size
        mem_device.size = node_config.far_memory_size

        # Add memory device to experiment
        e.add_memdev(mem_device)

        assert int(mem_device.addr/(1024*1024*1024)) == start_address_gb

        if use_pics:
            # Create pics for communication
            setup_pics(e, node_config, mem_device, host)
        else:
            # Directly add memory device to host
            setup_far_memory_without_pics(e, node_config, mem_device, host)


    else:
        print("sat far memory size to 0")
        # Nothin to do here because there is no far memory
    
    return e


    

    



    


