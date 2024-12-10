
from pyexps.pods.base import create_expermient

experiments = [
    create_expermient(
        'configurable_pod_4_node',
        use_pics=True,
        use_far_off_memory=True,
        start_address_gb=11,
    )
]
