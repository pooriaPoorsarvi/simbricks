
from pyexps.pods.base import create_expermient

experiments = [
    create_expermient(
        'configurable_pod_2_node_read',
        use_pics=False,
        use_far_off_memory=True,
        start_address_gb=11,
    ),
    create_expermient(
        'configurable_pod_4_node_read',
        use_pics=True,
        use_far_off_memory=True,
        start_address_gb=11,
    ),
    create_expermient(
        'direct_access_memory_read',
        use_pics=False,
        use_far_off_memory=False,
        start_address_gb=11,
    ),


    create_expermient(
        'configurable_pod_2_node_write',
        use_pics=False,
        use_far_off_memory=True,
        start_address_gb=11,
        is_read=False,
    ),
    create_expermient(
        'configurable_pod_4_node_write',
        use_pics=True,
        use_far_off_memory=True,
        start_address_gb=11,
        is_read=False,
    ),
    create_expermient(
        'direct_access_memory_write',
        use_pics=False,
        use_far_off_memory=False,
        start_address_gb=11,
        is_read=False,
    )
]
