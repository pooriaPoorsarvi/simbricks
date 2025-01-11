
from pyexps.pods.base import create_expermient

experiments = [
    create_expermient(
        'direct_access_memory',
        use_pics=False,
        use_far_off_memory_through_simbricks=False,
        start_address_gb=15,
        far_memory_size_gb=1,
        total_memory_gb=16,
    ),
    create_expermient(
        'direct_access_memory',
        use_pics=False,
        use_far_off_memory_through_simbricks=False,
        start_address_gb=15,
        far_memory_size_gb=1,
        total_memory_gb=16,
        is_read=False,
    )
]
