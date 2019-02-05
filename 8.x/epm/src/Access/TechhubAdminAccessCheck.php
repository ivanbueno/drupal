<?php

namespace Drupal\epm\Access;

use Drupal\Core\Access\AccessResult;
use Drupal\Core\Routing\Access\AccessInterface;
use Drupal\Core\Session\AccountInterface;
use Symfony\Component\Routing\Route;
use Symfony\Component\HttpFoundation\Request;

/**
 * Class TechhubAdminAccessCheck.
 *
 * @package Drupal\epm\Access
 */
class TechhubAdminAccessCheck implements AccessInterface {

    /**
     * Custom access check for hubs.
     *
     * @param Route $route
     * @param Request $request
     * @param AccountInterface $account
     * @return \Drupal\Core\Access\AccessResultAllowed|\Drupal\Core\Access\AccessResultForbidden
     */
    public function access(Route $route, Request $request, AccountInterface $account) {
        /* @var \Drupal\taxonomy\TermInterface $current_hub */
        $current_hub = $request->get('taxonomy_term');
        $access_key = $request->get('access_key');

        /* @var \Drupal\epm\Service\Techhub $techhub */
        $techhub = \Drupal::service('epm.techhub');
        if ($techhub->access($current_hub, $account, $access_key)) {
            return AccessResult::allowed();
        }
        return AccessResult::forbidden();
    }

}