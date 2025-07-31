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

import csv, random, sys, subprocess

def generate_electricidad_csv(filename="electricidad.csv", num_rows=100):
    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([
            "row_id", "cost", "output", "labor", "laborshare",
            "capital", "capitalshare", "fuel", "fuelshare"
        ])

        for i in range(1, num_rows + 1):
            output = random.randint(2, 10)
            labor = round(random.uniform(1.5, 3.5), 2)
            capital = random.randint(150, 200)
            fuel = round(random.uniform(15, 40), 1)

            laborshare = round(random.uniform(0.1, 0.4), 4)
            capitalshare = round(random.uniform(0.4, 0.7), 4)
            fuelshare = round(1.0 - laborshare - capitalshare, 4)
            fuelshare = max(0.0, min(fuelshare, 1.0))

            cost = round(
                labor * laborshare +
                capital * capitalshare +
                fuel * fuelshare, 3
            )

            writer.writerow([
                i, cost, output, labor, laborshare,
                capital, capitalshare, fuel, fuelshare
            ])

    try:
        subprocess.run(["dos2unix", filename], check=True)
        print(f"Archivo '{filename}' convertido a formato Unix.")
    except Exception as e:
        print(f"Error al ejecutar dos2unix: {e}")

    print(f"CSV generado: '{filename}' ({num_rows} filas).")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Uso: python elect.py <número_de_filas>")
        sys.exit(1)

    try:
        num_rows = int(sys.argv[1])
    except ValueError:
        print("Error: el argumento debe ser un número entero.")
        sys.exit(1)

    generate_electricidad_csv(num_rows=num_rows)
