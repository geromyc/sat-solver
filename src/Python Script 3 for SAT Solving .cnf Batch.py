import os 
import subprocess
import sys
import time
from datetime import datetime

# Configuration options for the SAT solver
USE_VSIDS = True
USE_LEARNING = True
USE_NONCHRONO = True
USE_WATCHED_LITS = True
USE_ACTIVITY_DECAY = True

def generate_log_filename():
    """Generate a shortened log filename based on the environment variable settings."""
    settings = [
        f"V-{str(USE_VSIDS)[0].upper()}",
        f"L-{str(USE_LEARNING)[0].upper()}",
        f"N-{str(USE_NONCHRONO)[0].upper()}",
        f"W-{str(USE_WATCHED_LITS)[0].upper()}",
        f"A-{str(USE_ACTIVITY_DECAY)[0].upper()}"
    ]
    timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    log_filename = f"SAT_Run_{timestamp}_{'_'.join(settings)}.log"
    return log_filename

def run_on_cnf_files(program_path, folder_path, log_file=None, timeout=300):
    if not os.path.isfile(program_path):
        print(f"Error: Program not found at {program_path}")
        return

    if not os.path.isdir(folder_path):
        print(f"Error: Folder not found at {folder_path}")
        return

    # Prepare environment with configuration options
    env = os.environ.copy()
    env['USE_VSIDS'] = 'true' if USE_VSIDS else 'false'
    env['USE_LEARNING'] = 'true' if USE_LEARNING else 'false'
    env['USE_NONCHRONO'] = 'true' if USE_NONCHRONO else 'false'
    env['USE_WATCHED_LITS'] = 'true' if USE_WATCHED_LITS else 'false'
    env['USE_ACTIVITY_DECAY'] = 'true' if USE_ACTIVITY_DECAY else 'false'

    # If no log filename is passed, generate one based on the environment settings
    if log_file is None:
        log_file = generate_log_filename()

    with open(log_file, "w", encoding="utf-8") as log:
        log.write(f"=== Batch Run Started at {datetime.now().isoformat()} ===\n")

        # Log the environment variable settings
        log.write(f"\n=== Environment Variables ===\n")
        log.write(f"USE_VSIDS: {env['USE_VSIDS']}\n")
        log.write(f"USE_LEARNING: {env['USE_LEARNING']}\n")
        log.write(f"USE_NONCHRONO: {env['USE_NONCHRONO']}\n")
        log.write(f"USE_WATCHED_LITS: {env['USE_WATCHED_LITS']}\n")
        log.write(f"USE_ACTIVITY_DECAY: {env['USE_ACTIVITY_DECAY']}\n")
        log.write(f"===============================\n")

        # Print the environment variable settings to console as well
        print(f"\n=== Environment Variables ===")
        print(f"USE_VSIDS: {env['USE_VSIDS']}")
        print(f"USE_LEARNING: {env['USE_LEARNING']}")
        print(f"USE_NONCHRONO: {env['USE_NONCHRONO']}")
        print(f"USE_WATCHED_LITS: {env['USE_WATCHED_LITS']}")
        print(f"USE_ACTIVITY_DECAY: {env['USE_ACTIVITY_DECAY']}")
        print(f"===============================\n")

        for filename in sorted(os.listdir(folder_path)):
            if filename.endswith(".cnf"):
                file_path = os.path.join(folder_path, filename)
                start_time = time.time()
                timestamp = datetime.now().isoformat()

                log.write(f"\n=== Running on {filename} at {timestamp} ===\n")
                print(f"Running on {filename}...")

                try:
                    proc = subprocess.Popen(
                        [program_path, file_path],
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE,
                        text=True,
                        env=env  # Pass the environment variables here
                    )

                    try:
                        stdout, stderr = proc.communicate(timeout=timeout)
                        duration = time.time() - start_time

                        log.write(stdout)
                        if stderr:
                            log.write(f"\n[stderr]\n{stderr}")
                        log.write(f"\n[Execution Time] {duration:.2f} seconds\n")

                    except subprocess.TimeoutExpired:
                        proc.kill()
                        log.write(f"\n[ERROR] Timeout after {timeout} seconds on {filename}\n")
                        # Do not call communicate() again — just consume output safely
                        try:
                            stdout, stderr = proc.stdout.read(), proc.stderr.read()
                            log.write(stdout)
                            if stderr:
                                log.write(f"\n[stderr]\n{stderr}")
                        except Exception:
                            log.write("[ERROR] Could not read stdout/stderr after timeout.\n")

                        print(f"Timeout on {filename} after {timeout} seconds.")

                except Exception as e:
                    log.write(f"\n[ERROR] Exception on {filename}: {e}\n")

        log.write(f"\n=== Batch Run Ended at {datetime.now().isoformat()} ===\n")

    print(f"\nAll results written to {log_file}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python run_on_cnf_files.py /path/to/program /path/to/folder")
    else:
        run_on_cnf_files(sys.argv[1], sys.argv[2], timeout=300)
