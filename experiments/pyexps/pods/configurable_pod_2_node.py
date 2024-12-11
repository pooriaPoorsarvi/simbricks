
from pyexps.pods.base import create_expermient

experiments = [
    # 4 GB far memory
    create_expermient(
        'configurable_pod_2_node',
        use_pics=False,
        use_far_off_memory_through_simbricks=True,
        start_address_gb=12,
        far_memory_size_gb=4,
        total_memory_gb=16,
    ),
    create_expermient(
        'configurable_pod_2_node',
        use_pics=False,
        use_far_off_memory_through_simbricks=True,
        start_address_gb=12,
        far_memory_size_gb=4,
        total_memory_gb=16,
        is_read=False,
    ),

    # 1 GB far memory
    create_expermient(
        'configurable_pod_2_node',
        use_pics=False,
        use_far_off_memory_through_simbricks=True,
        start_address_gb=15,
        far_memory_size_gb=1,
        total_memory_gb=16,
    ),
    create_expermient(
        'configurable_pod_2_node',
        use_pics=False,
        use_far_off_memory_through_simbricks=True,
        start_address_gb=15,
        far_memory_size_gb=1,
        total_memory_gb=16,
        is_read=False,
    ),
]
