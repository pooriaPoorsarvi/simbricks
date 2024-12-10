
from pyexps.pods.base import create_expermient

experiments = [
    create_expermient(
        'direct_access_memory',
        use_pics=False,
        use_far_off_memory=False,
        start_address_gb=11,
    )
]
