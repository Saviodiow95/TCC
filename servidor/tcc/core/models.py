from django.conf import settings
from django.db import models


class Historico(models.Model):
    temperatura = models.DecimalField(verbose_name='Temperatura (°C)', max_digits=10, decimal_places=2, null=True, blank=True)
    ph = models.DecimalField(verbose_name='pH', max_digits=10, decimal_places=2, null=True, blank=True)
    condutividade = models.DecimalField(verbose_name='Condutividade da Água (µS/cm)', max_digits=10, decimal_places=2, null=True, blank=True)
    nivel_racao = models.DecimalField(verbose_name='Nível de Ração (%)', max_digits=10, decimal_places=2, null=True, blank=True)
    horario = models.DateTimeField(verbose_name='Horário')
    usuario = models.ForeignKey(
        settings.AUTH_USER_MODEL,
        on_delete=models.CASCADE,
        verbose_name='Usuário',
        related_name='historicos'
    )

    class Meta:
        ordering = ['-horario']
        verbose_name = 'Histórico'
        verbose_name_plural = 'Históricos'

    def __str__(self):
        return f"Registro {self.id} - {self.horario} ({self.usuario.username})"
