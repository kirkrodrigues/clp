import argparse
import logging
from pathlib import Path
import sys

# Setup console logging
logging_console_handler = logging.StreamHandler()
logging_formatter = logging.Formatter(
    "%(asctime)s.%(msecs)03d %(levelname)s [%(module)s] %(message)s", datefmt="%Y-%m-%dT%H:%M:%S")
logging_console_handler.setFormatter(logging_formatter)
# Setup root logger
root_logger = logging.getLogger()
root_logger.setLevel(logging.INFO)
root_logger.addHandler(logging_console_handler)
# Create logger
logger = logging.getLogger(__name__)


def main(argv):
    args_parser = argparse.ArgumentParser(description="Program to TODO.")
    args_parser.add_argument("--src-dir", required=True, help="Argument description.")

    parsed_args = args_parser.parse_args(argv[1:])
    src_dir = Path(parsed_args.src_dir).absolute()

    for input_file_path in src_dir.rglob("*.hpp"):
        output_file_path = Path("/") / "tmp" / input_file_path.name
        parts = input_file_path.relative_to(src_dir).parts
        define_guard = "_".join([part.upper() for part in parts]).replace(".", "_")

        with open(input_file_path, "r") as input_file, open(output_file_path, "w") as output_file:
            for line in input_file:
                if line.startswith("#ifndef"):
                    output_file.write(f"#ifndef {define_guard}\n")
                elif line.startswith("#define"):
                    output_file.write(f"#define {define_guard}\n")
                elif line.startswith("#endif  //"):
                    output_file.write(f"#endif  // {define_guard}\n")
                else:
                    output_file.write(line)

        output_file_path.rename(input_file_path)

    return 0


if "__main__" == __name__:
    sys.exit(main(sys.argv))
