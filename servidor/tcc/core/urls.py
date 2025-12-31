
from django.urls import path, include
from django.contrib.auth import views as auth_views
from core.api.views import HistoricoApiView
from rest_framework import routers
from core.views import historico_view
from .views import dashboard_grafico, DashboardView
from rest_framework.authtoken.views import obtain_auth_token
router = routers.SimpleRouter()
router.register(r'historico', HistoricoApiView, basename='historico')

urlpatterns = [
    path('', historico_view, name='historico'),
    path('api/', include((router.urls, 'core'))),
    path('dashboard/', DashboardView.as_view(), name='dashboard'),
    path("dashboard-grafico/", dashboard_grafico, name="dashboard_grafico"),
    path('api-token-auth/', obtain_auth_token, name='api_token_auth'),
    path('accounts/login/', auth_views.LoginView.as_view(
        template_name='registration/login.html',
        redirect_authenticated_user=True
    ), name='login'),
    path('accounts/logout/', auth_views.LogoutView.as_view(next_page='login'), name='logout'),
    
]
