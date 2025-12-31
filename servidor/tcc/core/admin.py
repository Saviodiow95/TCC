from django.contrib import admin
from .models import Historico

# Register your models here.

@admin.register(Historico)
class HistoricoAdmin(admin.ModelAdmin):
    list_display = ['temperatura', 'ph', 'condutividade', 'nivel_racao', 'horario']
    list_filter = ['horario']
