(function ($, Drupal, drupalSettings) {

    Drupal.epm = Drupal.epm || {};

    drupalSettings.epmRefreshView = {};
    // drupalSettings.epmRefreshView['.view-techhub-visitors'] = {};
    // drupalSettings.epmRefreshView['.view-techhub-visitors'].interval = 5000;
    // drupalSettings.epmRefreshView['.view-techhub-charts-ticket-categories'] = {};
    // drupalSettings.epmRefreshView['.view-techhub-charts-ticket-categories'].interval = 5000;

    drupalSettings.epmRefreshView['ping_node_update'] = {};
    drupalSettings.epmRefreshView['ping_node_update'].interval = 5000;

    Drupal.behaviors.epmCore = {
        attach: function(context, settings) {

            // Close timers on page unload.
            window.addEventListener('unload', function(event) {
                $.each(drupalSettings.epmRefreshView, function(index, entry) {
                    clearTimeout(entry.timer);
                });
            });

            $.each(drupalSettings.epmRefreshView, function(target, entry) {
                // console.log('ATTACH: ' + target);
                clearTimeout(drupalSettings.epmRefreshView[target].timer);
                Drupal.epm.timer(target, entry.interval, context);
            });

        }
    }

    Drupal.epm.timer = function(target, refreshInterval, context) {
        drupalSettings.epmRefreshView[target].timer = setTimeout(function() {
            // console.log('CALL: ' + target + " : " + refreshInterval);
            clearTimeout(drupalSettings.epmRefreshView[target].timer);
            Drupal.epm.refresh(target, refreshInterval, context);
        }, refreshInterval);
    }

    Drupal.epm.refresh = function(target, refreshInterval, context) {
        // console.log('REFRESH: ' + target + " : " + refreshInterval);

        $.ajax({
            url: drupalSettings.path.baseUrl + 'techhub/ping/' + drupalSettings.epm.tid + '/' + drupalSettings.epmRefreshView[target].timestamp,
            success: function(response) {
                if (response.pong && parseInt(response.pong) > 0) {
                    drupalSettings.epmRefreshView[target].timestamp = Math.round(+new Date()/1000);
                    if(Drupal.AjaxCommands){
                        Drupal.AjaxCommands.prototype.viewsScrollTop = null;
                    }
                    //$(target).trigger('RefreshView');
                    $('.view-techhub-visitors').trigger('RefreshView');
                    $('.view-techhub-charts-ticket-categories').trigger('RefreshView');
                    $('.view-techhub-charts-visitors').trigger('RefreshView');
                }

                clearTimeout(drupalSettings.epmRefreshView[target].timer);
                Drupal.epm.timer(target, refreshInterval, context);

            },
            error: function(xhr) {},
            dataType: 'json'
        });
    }

})(jQuery, Drupal, drupalSettings);