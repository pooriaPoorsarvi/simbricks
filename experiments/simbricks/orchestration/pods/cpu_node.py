class CPUNode:

    def __init__(self, cores: int, cpu_type: str, internal_mem: int, far_mem: int):
        self.cores = cores
        self.cpu_type = cpu_type
        self.internal_mem = internal_mem
        self.far_mem = far_mem
    
    def __str__(self):
        return f'CPUNode(cores={self.cores}, cpu_type={self.cpu_type}, internal_mem={self.internal_mem}, far_mem={self.far_mem})'



