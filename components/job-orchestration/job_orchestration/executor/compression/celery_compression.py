from celery import Celery
from job_orchestration.config.celery.compression import celeryconfig  # type: ignore

app = Celery("compression")
app.config_from_object(celeryconfig)
