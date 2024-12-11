
from pyexps.pods.base import create_expermient

experiments = [
    create_expermient(
        'configurable_pod_4_node_read',
        use_pics=True,
        use_far_off_memory_through_simbricks=True,
        start_address_gb=12,
        far_memory_size_gb=4,
        total_memory_gb=16,
    ),
    create_expermient(
        'configurable_pod_4_node_write',
        use_pics=True,
        use_far_off_memory_through_simbricks=True,
        start_address_gb=12,
        far_memory_size_gb=4,
        total_memory_gb=16,
        is_read=False,
    ),
]
