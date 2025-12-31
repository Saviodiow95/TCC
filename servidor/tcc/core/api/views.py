from core.models import Historico
from core.api.serializers import HistoricoSerializer
from django.http import Http404
from rest_framework.views import APIView
from rest_framework.response import Response
from rest_framework import status
from rest_framework.viewsets import ModelViewSet
from rest_framework.permissions import IsAuthenticated

class HistoricoApiView(ModelViewSet):
    """
    A simple ViewSet for viewing and editing accounts.
    """
    #queryset = Historico.objects.all()
    serializer_class = HistoricoSerializer
    permission_classes = [IsAuthenticated]
    # permission_classes = [IsAccountAdminOrReadOnly]

    def perform_create(self, serializer):
        serializer.save(usuario=self.request.user)

    def get_queryset(self):
        return Historico.objects.filter(usuario=self.request.user)