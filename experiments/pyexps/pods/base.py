
import simbricks.orchestration.experiments as exp
import simbricks.orchestration.nodeconfig as nodec
import simbricks.orchestration.simulators as sim
from simbricks.orchestration.simulators import QemuCPUNodeHost, CPUPIC, MemPIC



class MemTest(nodec.AppConfig):

    def __init__(self, 
                 total_memory_gb: int,
                 start_address: int,
                 is_read: bool = True,
                 is_one_dd: bool = False,
                 on_dd_size_mb: int = 1,
                ):
        self.start_address = start_address
        self.is_read = is_read
        self.total_memory_gb = total_memory_gb
        self.is_one_dd = is_one_dd
        self.on_dd_size_kb = on_dd_size_mb

    def run_cmds(self, node):
        main_command = ''
        if not self.is_one_dd:
            # since bs is in 1G, the address difference is the count
            count = int(self.total_memory_gb - self.start_address)
            bs='1G'
            start_address = int(self.start_address)
        else:
            count = 1
            bs=f'{self.on_dd_size_kb}M'
            # start address will bi the same, but since it's done through skips, we need to convert
            start_address = int((self.start_address * 1024 )/self.on_dd_size_kb)

        if self.is_read:
            main_command = f'dd if=/dev/mem bs={bs} skip={start_address} count={count} status=progress | xxd | less'
        else:
            main_command = f'dd if=/dev/zero of=/dev/mem bs={bs} seek={start_address} count={count} status=progress'
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
    use_far_off_memory_through_simbricks: bool = True,
    start_address_gb: int = 11,
    total_memory_gb: int = 12,
    far_memory_size_gb: int = 1,
    is_read: bool = True,
    only_use_custom_memory: bool = False,
    is_one_dd: bool = False,
    on_dd_size_mb: int = 1,
) -> exp.Experiment:

    # create proper experiment name
    access_size = "1G" if not is_one_dd else f"{on_dd_size_mb}M"
    experiment_name = f'{experiment_name}_{total_memory_gb-start_address_gb}G_{access_size}_{"read" if is_read else "write"}'

    # Create node
    node_config = nodec.NodeConfig()
    node_config.should_accelerate = False
    node_config.memory = total_memory_gb * 1024
    node_config.nockp = True
    node_config.app = MemTest(
        total_memory_gb=total_memory_gb,
        start_address=start_address_gb,
        is_read=is_read,
        is_one_dd=is_one_dd,
        on_dd_size_mb=on_dd_size_mb,
    )
    node_config.only_use_custom_memory = only_use_custom_memory
    

    # TODO This if else is getting a bit messy, should rethink it a bit 
    if use_far_off_memory_through_simbricks or only_use_custom_memory:
        # Need to do this if condition first instead of with the rest because it's
        # used in the host to setup it's CMD. TODO see if we can make this cleaner
        node_config.far_memory_size = far_memory_size_gb * 1024 * 1024 * 1024

    # Create experiment
    e = exp.Experiment(experiment_name)
    e.checkpoint = False

    # Create host
    host = QemuCPUNodeHost(node_config)
    host.name = 'host.0.qemu'
    host.wait = True
    
    # Add host to experiment
    e.add_host(host)
    
    if use_far_off_memory_through_simbricks:
        assert only_use_custom_memory == False

    if only_use_custom_memory:
        assert use_far_off_memory_through_simbricks == False
        # No memory device so just return
        return e
    


    if use_far_off_memory_through_simbricks:
        # Set far memory size
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


    

    



    


