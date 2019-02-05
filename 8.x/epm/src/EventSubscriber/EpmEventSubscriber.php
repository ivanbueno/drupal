<?php
namespace Drupal\epm\EventSubscriber;

use Symfony\Component\HttpKernel\KernelEvents;
use Symfony\Component\HttpKernel\Event\GetResponseEvent;
use Symfony\Component\EventDispatcher\EventSubscriberInterface;
use Symfony\Component\HttpFoundation\RedirectResponse;

class EpmEventSubscriber implements EventSubscriberInterface {

    public function onKernelRequest(GetResponseEvent $event) {

        // Redirect Hub taxonomy terms to custom templates
        $term = \Drupal::routeMatch()->getParameter('taxonomy_term');
        if (\Drupal::routeMatch()->getRouteName() == 'entity.taxonomy_term.canonical'
            && $term
            && $term->getVocabularyId() == 'hubs') {

            $response = new RedirectResponse(\Drupal::url(
                'epm.hub',
                ['taxonomy_term' => $term->id()]
            )
            );
            return $response->send();
        }
    }

    /**
     * {@inheritdoc}
     */
    public static function getSubscribedEvents() {
        return [
            KernelEvents::REQUEST => 'onKernelRequest',
        ];
    }
}