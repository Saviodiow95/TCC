from datetime import datetime

from django.contrib.auth.decorators import login_required
from django.contrib.auth.mixins import LoginRequiredMixin
from django.shortcuts import render
from django.views.generic import View

from .models import Historico


@login_required
def historico_view(request):
    historicos = Historico.objects.filter(usuario=request.user).order_by("-horario")
    return render(request, "core/historico.html", {"historicos": historicos})


class DashboardView(LoginRequiredMixin, View):
    def get(self, request):
        inicio = request.GET.get("inicio") or ""
        fim = request.GET.get("fim") or ""

        historicos = Historico.objects.filter(usuario=request.user)
        # Filtro opcional por período
        try:
            if inicio:
                data_inicio = datetime.strptime(inicio, "%Y-%m-%d").date()
                historicos = historicos.filter(horario__date__gte=data_inicio)
            if fim:
                data_fim = datetime.strptime(fim, "%Y-%m-%d").date()
                historicos = historicos.filter(horario__date__lte=data_fim)
        except ValueError:
            pass

        historicos = historicos.order_by("-horario")
        return render(
            request,
            "core/dashboard.html",
            {"historicos": historicos, "inicio": inicio, "fim": fim},
        )


@login_required
def dashboard_grafico(request):
    inicio = request.GET.get("inicio") or ""
    fim = request.GET.get("fim") or ""

    historicos = Historico.objects.all()
    try:
        if inicio:
            data_inicio = datetime.strptime(inicio, "%Y-%m-%d").date()
            historicos = historicos.filter(horario__date__gte=data_inicio)
        if fim:
            data_fim = datetime.strptime(fim, "%Y-%m-%d").date()
            historicos = historicos.filter(horario__date__lte=data_fim)
    except ValueError:
        pass

    historicos = historicos.order_by("horario")

    # Preparar listas para os gráficos
    horarios = [h.horario.strftime("%d/%m %H:%M") for h in historicos]
    temperaturas = [float(h.temperatura or 0) for h in historicos]
    phs = [float(h.ph or 0) for h in historicos]
    condutividades = [float(h.condutividade or 0) for h in historicos]
    niveis_racao = [float(h.nivel_racao or 0) for h in historicos]

    context = {
        "horarios": horarios,
        "temperaturas": temperaturas,
        "phs": phs,
        "condutividades": condutividades,
        "niveis_racao": niveis_racao,
        "inicio": inicio,
        "fim": fim,
    }
    return render(request, "core/dashboard_grafico2.html", context)
