from pydantic import BaseModel

class CPUNodeConfig(BaseModel):
    cores: int = 1
    cpu_type: str = 'q35'
    total_mem: int = 8192*2
    far_mem: int = 8192




class CPUNode:

    def __init__(self, node_config: CPUNodeConfig, **kwargs):
        '''
        Args:
            cores: number of cores
            cpu_type: type of the CPU
            total_mem: total memory in MB
            far_mem: far memory in MB
        '''
        self.node_config = node_config
        self.cores = self.node_config.cores
        self.cpu_type = self.node_config.cpu_type
        self.total_mem = self.node_config.total_mem
        self.far_mem = self.node_config.far_mem
        if self.far_mem > 0:
            assert self.far_mem <= self.total_mem
            # 8GB of memory is reserved for the system more explanation in the qemu project
            assert self.total_mem - self.far_mem >= 8192
        print("sat up the cpu node as ", self)
    
    def __str__(self):
        return f'CPUNode(cores={self.cores}, cpu_type={self.cpu_type}, total_mem={self.total_mem}, far_mem={self.far_mem})'



