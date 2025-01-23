#!/usr/bin/env python3
import re
import csv
from collections import defaultdict

def parse_gainsight_line(line):
    pattern = re.compile(
        r'^Gainsight\s+'
        r'(Write|Read)\s+at\s+Bank\s+(\d+)\s+'
        r'at\s+address\s+(\d+)\s+'
        r'at\s+time\s+stamp:\s+(\d+)\s*ns\s+with\s+data\s+(\S+)'
    )
    match = pattern.match(line.strip())
    if match:
        operation = match.group(1)        # "Write" or "Read"
        bank_str  = match.group(2)        # e.g. "7"
        addr_str  = match.group(3)        # e.g. "7"
        ts_str    = match.group(4)        # e.g. "170"
        # data_str = match.group(5)       # e.g. "1" (if needed)

        bank      = int(bank_str)
        address   = int(addr_str)
        timestamp = int(ts_str)
        return (operation, bank, address, timestamp)
    return None

def load_and_sort_events(filename):
    events = []
    with open(filename, 'r') as f:
        for line in f:
            if not line.startswith("Gainsight"):
                continue
            parsed = parse_gainsight_line(line)
            if parsed is None:
                continue
            (operation, bank, address, timestamp) = parsed
            events.append((timestamp, operation, bank, address))

    # Sort by timestamp ascending
    events.sort(key=lambda x: x[0])
    return events

def compute_lifetimes(events):
    bankaddr_events = defaultdict(list)

    # 1) Group events by (bank, address)
    for (ts, op, bank, addr) in events:
        bankaddr_events[(bank, addr)].append((ts, op))

    # 2) For each (bank, addr), compute lifetimes
    lifetime_map = {}

    for (bankaddr, ev_list) in bankaddr_events.items():
        lifetimes = []
        current_write_time = None
        last_read_after_write = None

        for (ts, op) in ev_list:
            if op == "Write":
                # Finalize previous write (if any) with the last read
                if current_write_time is not None:
                    if last_read_after_write is not None and last_read_after_write > current_write_time:
                        lifetime = last_read_after_write - current_write_time
                        lifetimes.append(lifetime)
                # Start a new write
                current_write_time = ts
                last_read_after_write = None

            elif op == "Read":
                if current_write_time is not None and ts > current_write_time:
                    last_read_after_write = ts

        # Finalize the very last write if it had any subsequent read
        if current_write_time is not None and last_read_after_write is not None:
            if last_read_after_write > current_write_time:
                lifetimes.append(last_read_after_write - current_write_time)

        lifetime_map[bankaddr] = lifetimes

    return lifetime_map

def main():
    log_file = "output.log"  # Adjust as needed
    events = load_and_sort_events(log_file)
    lifetime_map = compute_lifetimes(events)

    # Sort keys by (bank, address)
    sorted_keys = sorted(lifetime_map.keys(), key=lambda x: (x[0], x[1]))

    print("Data Lifetimes (time = last-read - write), grouped by (bank, address):")

    for (bank, addr) in sorted_keys:
        lifetimes = lifetime_map[(bank, addr)]
        # Only print if there's at least one lifetime
        if lifetimes:
            print(f"Bank={bank}, Addr={addr}: lifetimes = {lifetimes}")

    # Write to CSV, ignoring empty lifetimes
    csv_filename = "lifetimes_bank_addr.csv"
    with open(csv_filename, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["Bank", "Address", "Lifetime"])
        for (bank, addr) in sorted_keys:
            lifetimes = lifetime_map[(bank, addr)]
            if lifetimes:
                for lt in lifetimes:
                    writer.writerow([bank, addr, lt])

    print(f"\nLifetimes saved to '{csv_filename}'. (Only non-empty lifetimes included.)")

    # Bin them into every 100ns

if __name__ == "__main__":
    main()
