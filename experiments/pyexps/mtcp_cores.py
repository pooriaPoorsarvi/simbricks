import modes.experiments as exp
import modes.simulators as sim
import modes.nodeconfig as node


server_cores_configs = [1, 2, 4, 8]
stacks = ['linux', 'mtcp']
client_cores = 1
num_clients = 1
connections = 128
msg_size = 64

experiments = []
for server_cores in server_cores_configs:
  for stack in stacks:
    e = exp.Experiment('qemu-ib-switch-mtcp_cores-%s-%d' % (stack,server_cores))
    e.timeout = 5* 60
    # add meta data for output file
    e.metadata['msg_size'] = msg_size
    e.metadata['stack'] = stack

    net = sim.SwitchNet()
    e.add_network(net)

    if stack == 'tas':
        n = node.TASNode
    elif stack == 'mtcp':
        n = node.MtcpNode
    else:
        n = node.I40eLinuxNode

    servers = sim.create_basic_hosts(e, 1, 'server', net, sim.I40eNIC, sim.QemuHost,
            n, node.RPCServer)

    clients = sim.create_basic_hosts(e, num_clients, 'client', net, sim.I40eNIC,
            sim.QemuHost, n, node.RPCClient, ip_start = 2)

    for h in servers:
        h.node_config.cores = server_cores
        h.node_config.app.threads = server_cores
        h.node_config.app.max_flows = connections * 4
        h.sleep = 5



    for c in clients:
        c.wait = True
        c.node_config.cores = client_cores
        c.node_config.app.threads = client_cores

        c.node_config.app.server_ip = servers[0].node_config.ip
        c.node_config.app.max_msgs_conn = 1
        c.node_config.app.max_flows = \
            int(connections / num_clients / client_cores)

    for h in servers + clients:
        h.node_config.app.max_bytes = msg_size

        if stack == 'linux':
            h.node_config.disk_image = 'tas'
        elif stack == 'tas':
            c.node_config.cores += 2
            c.node_config.fp_cores = 1
    experiments.append(e)
