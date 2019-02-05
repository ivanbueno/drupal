<?php
/**
 * @file
 * Contains \Drupal\epm\Routing\RouteSubscriber.
 */
namespace Drupal\epm\Routing;

use Drupal\Core\Routing\RouteSubscriberBase;
use Symfony\Component\Routing\RouteCollection;

class RouteSubscriber extends RouteSubscriberBase {
    /**
     * {@inheritdoc}
     */
    public function alterRoutes(RouteCollection $collection) {
//        if ($route = $collection->get('entity.taxonomy_vocabulary.overview_form')) {
//            $route->setDefault('_form', '\Drupal\epm\Form\OverviewTerms');
//        }
    }
}