import argparse
import logging
import pathlib
import shutil
import subprocess
import sys

# Setup logging
# Create logger
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)
# Setup console logging
logging_console_handler = logging.StreamHandler()
logging_formatter = logging.Formatter("%(asctime)s [%(levelname)s] %(message)s")
logging_console_handler.setFormatter(logging_formatter)
logger.addHandler(logging_console_handler)


def main(argv):
    args_parser = argparse.ArgumentParser(description="Run a basic compression and decompression test.")
    args_parser.add_argument("bin-dir", help="Directory containing binaries.")
    args_parser.add_argument("output-dir", help="Directory for test output.")
    args_parser.add_argument("dataset", help="Path to dataset to use for testing.")
    args = args_parser.parse_args(argv[1:])
    parsed_args = vars(args)
    bin_dir = pathlib.Path(parsed_args["bin-dir"]).resolve()
    output_dir = pathlib.Path(parsed_args["output-dir"]).resolve()
    dataset_path = pathlib.Path(parsed_args["dataset"]).resolve()

    compression_dir = output_dir / "compressed"
    compression_dir.mkdir(exist_ok=True, parents=True)
    decompression_dir = output_dir / "decompressed"

    # Clean-up previous test in case it failed
    if compression_dir.exists():
        shutil.rmtree(compression_dir)
    if decompression_dir.exists():
        shutil.rmtree(decompression_dir)

    # Compress
    cmd = [
        str(bin_dir / "clp"),
        "c", str(compression_dir), str(dataset_path)
    ]
    proc = subprocess.run(cmd)
    if 0 != proc.returncode:
        logger.error(f"Compression failed with command {' '.join(cmd)}")
        return -1

    # Decompress
    cmd = [
        str(bin_dir / "clp"),
        "x", str(compression_dir), str(decompression_dir)
    ]
    proc = subprocess.run(cmd)
    if 0 != proc.returncode:
        logger.error(f"Decompression failed with command {' '.join(cmd)}")
        return False

    # diff
    cmd = [
        "diff",
        "-r",
        str(decompression_dir / dataset_path.relative_to(dataset_path.anchor)),
        str(dataset_path)
    ]
    proc = subprocess.run(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    if 0 != proc.returncode:
        logger.error(f"Decompressed data doesn't match raw data. See output of:\n{' '.join(cmd)}")
        return False

    # Clean-up
    shutil.rmtree(decompression_dir)
    shutil.rmtree(compression_dir)

    return 0


if "__main__" == __name__:
    sys.exit(main(sys.argv))
