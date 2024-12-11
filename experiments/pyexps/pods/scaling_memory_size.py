
from pyexps.pods.base import create_expermient

experiments = []

for i in range(4):
    multiplier = 2 ** i

    experiments += [
        create_expermient(
            'node_scale',
            use_pics=True,
            use_far_off_memory_through_simbricks=True,
            start_address_gb=12,
            far_memory_size_gb=4,
            total_memory_gb=16,
            is_one_dd=True,
            on_dd_size_mb=128 * multiplier
        ),
        create_expermient(
            'node_scale',
            use_pics=True,
            use_far_off_memory_through_simbricks=True,
            start_address_gb=12,
            far_memory_size_gb=4,
            total_memory_gb=16,
            is_read=False,
            is_one_dd=True,
            on_dd_size_mb=128*multiplier
        ),
    ]
