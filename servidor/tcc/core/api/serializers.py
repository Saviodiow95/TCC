from rest_framework import serializers
from core.models import Historico

class HistoricoSerializer(serializers.ModelSerializer):
    usuario = serializers.StringRelatedField(read_only=True)
    class Meta:
        model = Historico
        fields = "__all__"

        