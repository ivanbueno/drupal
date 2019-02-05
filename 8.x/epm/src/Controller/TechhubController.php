<?php
/**
 * @file
 * Contains \Drupal\epm\Controller\TechhubController.
 */

namespace Drupal\epm\Controller;

use Drupal\Core\Session\AccountInterface;
use Symfony\Component\HttpFoundation\Request;
use Drupal\Core\Controller\ControllerBase;
use Drupal\taxonomy\TermInterface;
use Symfony\Component\HttpFoundation\RedirectResponse;
use Symfony\Component\HttpFoundation\JsonResponse;
use Drupal\Component\Utility\Xss;

class TechhubController extends ControllerBase {

    public function swipe(AccountInterface $user, Request $request) {
        /* @var \Drupal\epm\Service\Techhub $techhub */
        $techhub = \Drupal::service('epm.techhub');
        $hubs = $techhub->getHubsByAdmin(\Drupal::currentUser()->id());

        // Create ticket automatically if the agent only has one hub
        if (count($hubs) == 1) {
            $hub_key = key($hubs);
            $node = $techhub->createTicket($hub_key, $user);
            $response = new RedirectResponse('/techhub/hub/' . $hub_key);
            return $response->send();
        }

        // Let agent select which hub to check the customer in.
        return [
            'form' => \Drupal::formBuilder()->getForm(
                '\Drupal\epm\Form\TechhubSwipeForm',
                [
                    'hubs' => $hubs,
                    'user' => $user,
                ]
            ),
        ];
    }

    public function hub(TermInterface $taxonomy_term, Request $request) {
        /* @var \Drupal\epm\Service\Techhub $techhub */
        $techhub = \Drupal::service('epm.techhub');
        $productivity = $nps = $ticket_status = [];

        $parent = \Drupal::entityTypeManager()->getStorage('taxonomy_term')->loadParents($taxonomy_term->id());
        $parent = reset($parent);

        // Render certain fields
        $field_hub_hours = $taxonomy_term->field_hub_hours->view('full');
        $field_hub_address = $taxonomy_term->field_hub_address->view('full');
        $field_hub_content = $taxonomy_term->field_hub_content->view('full');

        // Load children if any
        $children = [];
        if (!$parent) {
            $children = \Drupal::entityTypeManager()->getStorage('taxonomy_term')->loadChildren($taxonomy_term->id());

            $productivity = $techhub->getProductivity(array_keys($children));
            $nps = $techhub->getFeedbackNps(array_keys($children));
            $ticket_status = $techhub->getStatusTickets(array_keys($children));
        }

        return [
            '#theme' => $parent ? 'epm_techhub_hub' : 'epm_techhub_org',
            '#content' => $taxonomy_term,
            '#parent' => $parent,
            '#children' => array_keys($children),
            '#stats' => [
                'productivity' => $productivity,
                'nps' => $nps,
                'ticket_status' => $ticket_status,
            ],
            '#fields' => [
                'field_hub_hours' => \Drupal::service('renderer')->renderRoot($field_hub_hours),
                'field_hub_address' => \Drupal::service('renderer')->renderRoot($field_hub_address),
                'field_hub_content' => \Drupal::service('renderer')->renderRoot($field_hub_content),
                'destination' => Xss::filter(\Drupal::request()->get('destination')),
            ],
            '#attached' => $parent ? [
                'library' => ['epm/epm-core'],
                'drupalSettings' => [
                    'epm' => [
                        'tid' => $taxonomy_term->id(),
                    ]
                ]
            ] : [],
        ];
    }

    public function getTitle(TermInterface $taxonomy_term) {
        $parent = \Drupal::entityTypeManager()->getStorage('taxonomy_term')->loadParents($taxonomy_term->id());
        $parent_label = '';
        if ($parent) {
            $parent = reset($parent);
            $parent_label = ' | ' . $parent->label();
        }

        return $taxonomy_term->label() . $parent_label;
    }

    public function userCard(AccountInterface $user) {

        $field_user_picture = $user->user_picture->view('full');

        return [
            '#theme' => 'epm_techhub_user',
            '#content' => $user,
            '#fields' => [
                'user_picture' => \Drupal::service('renderer')->renderRoot($field_user_picture),
            ],
        ];
    }

    public function pingNewTickets(TermInterface $taxonomy_term, $timestamp) {
        /* @var \Drupal\epm\Service\Techhub $techhub */
        $techhub = \Drupal::service('epm.techhub');
        return new JsonResponse(
            ["pong" => $techhub->latestUpdatedTicketsCount($taxonomy_term->id(), $timestamp)]
        );
    }
}