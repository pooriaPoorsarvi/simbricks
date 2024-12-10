
from pyexps.pods.base import create_expermient

experiments = [
    create_expermient(
        'configurable_pod_2_node',
        use_pics=False,
        use_far_off_memory=True,
        start_address_gb=11,
    )
]
