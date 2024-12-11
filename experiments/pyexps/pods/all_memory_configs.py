
from pyexps.pods.base import create_expermient
from pyexps.pods.configurable_pod_2_node import experiments as configurable_pod_2_node_experiments
from pyexps.pods.configurable_pod_4_node import experiments as configurable_pod_4_node_experiments
from pyexps.pods.direct_access import experiments as direct_access_experiments
from pyexps.pods.direct_without_fast_path import experiments as direct_without_fast_path_experiments

experiments = []
experiments += configurable_pod_2_node_experiments
experiments += configurable_pod_4_node_experiments
experiments += direct_access_experiments
experiments += direct_without_fast_path_experiments
