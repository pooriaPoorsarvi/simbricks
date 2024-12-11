
from pyexps.pods.base import create_expermient

experiments = [
    create_expermient(
        'direct_access_memory_read',
        use_pics=False,
        use_far_off_memory_through_simbricks=False,
        start_address_gb=12,
        far_memory_size_gb=4,
        total_memory_gb=16,
    ),
    create_expermient(
        'direct_access_memory_write',
        use_pics=False,
        use_far_off_memory_through_simbricks=False,
        start_address_gb=12,
        far_memory_size_gb=4,
        total_memory_gb=16,
        is_read=False,
    )
]
