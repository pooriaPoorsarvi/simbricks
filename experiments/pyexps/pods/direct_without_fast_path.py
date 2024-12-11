from pyexps.pods.base import create_expermient

experiments = [
    create_expermient(
        'direct_access_memory_without_fast_path',
        use_pics=False,
        use_far_off_memory_through_simbricks=False,
        start_address_gb=12,
        far_memory_size_gb=4,
        total_memory_gb=16,
        only_use_custom_memory=True,
    ),
    create_expermient(
        'direct_access_memory_without_fast_path',
        use_pics=False,
        use_far_off_memory_through_simbricks=False,
        start_address_gb=12,
        far_memory_size_gb=4,
        total_memory_gb=16,
        only_use_custom_memory=True,
        is_read=False,
    ),

    create_expermient(
        'direct_access_memory_without_fast_path',
        use_pics=False,
        use_far_off_memory_through_simbricks=False,
        start_address_gb=15,
        far_memory_size_gb=1,
        total_memory_gb=16,
        only_use_custom_memory=True,
    ),
    create_expermient(
        'direct_access_memory_without_fast_path',
        use_pics=False,
        use_far_off_memory_through_simbricks=False,
        start_address_gb=15,
        far_memory_size_gb=1,
        total_memory_gb=16,
        only_use_custom_memory=True,
        is_read=False,
    ),
]