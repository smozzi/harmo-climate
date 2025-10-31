# Contributor Guidance

- Keep `README.md` and `AGENTS.md` synchronized with the latest project expectations. Whenever one file is updated, review the other to ensure instructions and documentation stay aligned.
- Document new instructions or workflow changes promptly so future updates to both files remain coherent.
- Source code now lives under the `src/harmoclimate/` package. Follow the module boundaries introduced there (`data_ingest`, `training`, `template_cpp`, `display`, etc.) when adding new functionality.
- Run the pipeline via the CLI (`python main.py generate <NUM_POSTE>`). The `STATION_CODE` constant in `src/harmoclimate/config.py` only provides the default when no station code is supplied; downstream helpers still derive department codes and artefact paths automatically.
- Generated artefacts belong under the `generated/` directory tree. Do not check model outputs or filtered datasets into version control—use the existing `.gitignore` rules instead.
- Generated artefacts must share the basename `{country_code}_{station_slug}` (e.g. `fr_bourges`) with appropriate extensions for each output; the slug is derived from the dataset `NOM_USUEL`.
- The ingestion pipeline requires `pyarrow` and always writes Parquet datasets; CSV fallbacks are no longer supported.
- New Python modules should include a module-level docstring and prefer explicit relative imports within the `harmoclimate` package.
- Dependency management relies on `pyproject.toml`. Install the project (and optional extras) with `pip install .` or `uv pip install .`.
- Prefer the helper scripts in `scripts/` (`setup.sh`, `activate.sh`) for creating and reusing the local virtual environment when developing.
