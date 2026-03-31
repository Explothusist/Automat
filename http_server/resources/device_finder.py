import subprocess
import ipaddress
from concurrent.futures import ThreadPoolExecutor
import netifaces # pip install netifaces
import requests # pip install requests

def get_active_interface():
    for interface in netifaces.interfaces():
        addresses = netifaces.ifaddresses(interface)

        if netifaces.AF_INET in addresses:
            for address in addresses[netifaces.AF_INET]:
                ip = address.get('addr')
                if ip and not ip.startswith("127.") and (
                    ip.startswith("192.168.") or
                    ip.startswith("10.") or
                    ip.startswith("172.")
                ):
                    return interface, address
    return None, None

def ping(ip):
    result = subprocess.run(
        ["ping", "-n", "1", "-w", "1000", str(ip)],
        stdout=subprocess.DEVNULL
    )
    return str(ip) if result.returncode == 0 else None

interface, address = get_active_interface()

if not interface:
    raise Exception("No active network interface found")

print("Using Interface:", interface)

network = ipaddress.IPv4Network(
    f"{address['addr']}/{address['netmask']}",
    strict=False
)

active_ips = []

with ThreadPoolExecutor(max_workers=50) as executor:
    results = executor.map(ping, network.hosts())

for result in results:
    if result:
        active_ips.append(result)

print("Active IPs:", active_ips)

def check_endpoint(ip):
    try:
        url = f"http://{ip}/is_atmt"
        response = requests.get(url, timeout=2.0)

        if response.status_code == 418: # 418 I'm a teapot
            return {"ip": ip, "name": response.text}
    except:
        pass
    return None

with ThreadPoolExecutor(max_workers=20) as executor:
    results = executor.map(check_endpoint, active_ips)

valid_devices = [r for r in results if r]

print("Automat Devices:")
for device in valid_devices:
    print(f"    IP: {device["ip"]}  Name: {device["name"]}")


# # Run 'pip install python-nmap'
# import nmap

# def scan_network(network_range):
#     scanner = nmap.PortScanner()
#     scanner.scan(hosts=network_range, arguments='-sn')

#     hosts = []
#     for host in scanner.all_hosts():
#         if scanner[host].state() == 'up':
#             hosts.append(host)
    
#     return hosts


