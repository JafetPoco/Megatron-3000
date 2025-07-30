import csv, random, argparse
from datetime import datetime, timedelta

def generate_sensor_data_csv(
    filename="sensor_data.csv",
    num_rows=200000,
    timestamp_format="unix",
    num_sensors=20
):
    locations = [
        "MainControl",
        "BackupControl",
        "SwitchyardA",
        "SwitchyardB",
        "Bay01",
        "Bay02",
        "Bay03",
        "Bay04",
        "PanelRoomA",
        "PanelRoomB",
        "PanelRoomC",
        "TransformerZone1",
        "TransformerZone2",
        "CoolingUnitA",
        "CoolingUnitB",
        "BatteryStorage1",
        "BatteryStorage2",
        "RelayCabinet1",
        "RelayCabinet2",
        "SCADARoom",
        "UPSRoom",
        "BreakerHall",
        "SubstationEast",
        "SubstationWest",
        "MeteringUnit"
    ]
    sensor_ids = [f"sensor_{str(i+1).zfill(3)}" for i in range(num_sensors)]
    start_time = datetime.now() - timedelta(seconds=num_rows)

    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([
            "row_id","timestamp", "sensor_id", "voltage", "current", "power",
            "energy_consumed_kWh", "cost_per_kWh", "total_cost", "location"
        ])

        for i in range(num_rows):
            row_id = i+1
            sensor_id = random.randint(1,20)
            voltage = round(random.uniform(210, 240), 2)
            current = round(random.uniform(20.5, 100), 2)
            power = round(voltage * current, 2)
            energy = round((power / 1000.0), 6)
            cost = round(random.uniform(0.40, 0.90), 2)
            total_cost = round(energy * cost, 4)
            location = random.choice(locations)

            if timestamp_format == "iso8601":
                ts = (start_time + timedelta(seconds=i)).isoformat()
            elif timestamp_format == "unix":
                ts = int((datetime(2000, 1, 1) + timedelta(seconds=random.randint(0, int((datetime(2025, 1, 1) - datetime(2000, 1, 1)).total_seconds()) + i))).timestamp())
            else:
                ts = i  # si agregas opción "int"

            writer.writerow([
                row_id, ts, sensor_id, voltage, current, power,
                energy, cost, total_cost, location
            ])

    print(f"CSV generado: '{filename}' ({num_rows} filas).")

generate_sensor_data_csv()
