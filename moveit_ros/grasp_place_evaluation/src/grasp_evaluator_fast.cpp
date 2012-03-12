/*********************************************************************
*
*  Copyright (c) 2009, Willow Garage, Inc.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

// Author(s): E. Gil Jones

#include <grasp_place_evaluation/grasp_evaluator_fast.h>

using moveit_manipulation_msgs::GraspResult;
using moveit_msgs::MoveItErrorCodes;

namespace grasp_place_evaluation {

GraspEvaluatorFast::GraspEvaluatorFast(const planning_models::KinematicModelConstPtr& kmodel,
                                       const std::map<std::string, kinematics::KinematicsBasePtr>& solver_map)
  : GraspEvaluator(), 
    InterpolationEvaluator(kmodel, solver_map)
{  
}

// void GraspEvaluatorFast::testGrasp(const moveit_msgs::PickupGoal &pickup_goal,
//                                    const moveit_msgs::Grasp &grasp,
//                                    GraspExecutionInfo &execution_info)
// {
// ros::WallTime start = ros::WallTime::now();

// //always true
// execution_info.result_.continuation_possible = true;

// planning_environment::CollisionModels* cm = getCollisionModels();
// planning_models::KinematicState* state = getPlanningSceneState();

// std::vector<std::string> end_effector_links, arm_links; 
// getGroupLinks(handDescription().gripperCollisionName(pickup_goal.arm_name), end_effector_links);
// getGroupLinks(handDescription().armGroup(pickup_goal.arm_name), arm_links);

// collision_space::EnvironmentModel::AllowedCollisionMatrix original_acm = cm->getCurrentAllowedCollisionMatrix();
// cm->disableCollisionsForNonUpdatedLinks(handDescription().gripperCollisionName(pickup_goal.arm_name));
// collision_space::EnvironmentModel::AllowedCollisionMatrix group_disable_acm = cm->getCurrentAllowedCollisionMatrix();
// group_disable_acm.changeEntry(pickup_goal.collision_object_name, end_effector_links, true); 

// //turning off collisions for the arm associated with this end effector
// for(unsigned int i = 0; i < arm_links.size(); i++) {
//   group_disable_acm.changeEntry(arm_links[i], true);
// }

// cm->setAlteredAllowedCollisionMatrix(group_disable_acm);

// //first we apply link padding for grasp check

// cm->applyLinkPaddingToCollisionSpace(linkPaddingForGrasp(pickup_goal));

// //using pre-grasp posture, cause grasp_posture only matters for closing the gripper
// std::map<std::string, double> pre_grasp_joint_vals;    
// for(unsigned int j = 0; j < grasp.pre_grasp_posture.name.size(); j++) {
//   pre_grasp_joint_vals[grasp.pre_grasp_posture.name[j]] = grasp.pre_grasp_posture.position[j];
// }
// state->setKinematicState(pre_grasp_joint_vals);

// //assume for now grasp pose is in correct frame

// std_msgs::Header target_header;
// target_header.frame_id = pickup_goal.target.reference_frame_id;
// geometry_msgs::PoseStamped grasp_world_pose_stamped;
// cm->convertPoseGivenWorldTransform(*state,
//                                   cm->getWorldFrameId(),
//                                   target_header,
//                                   grasp.grasp_pose,
//                                   grasp_world_pose_stamped);
// tf::Transform grasp_pose;
// tf::poseMsgToTF(grasp_world_pose_stamped.pose, grasp_pose);
// state->updateKinematicStateWithLinkAt(handDescription().gripperFrame(pickup_goal.arm_name),grasp_pose);

// std_msgs::ColorRGBA col_grasp;
// col_grasp.r = 0.0;
// col_grasp.g = 1.0;
// col_grasp.b = 0.0;
// col_grasp.a = 1.0;

// if(cm->isKinematicStateInCollision(*state)) {
//   ROS_INFO_STREAM("Grasp pose in collision");
//   execution_info.result_.result_code = GraspResult::GRASP_IN_COLLISION;
// }

// //now we turn link paddings off for rest of the pose checks
// //cm->revertCollisionSpacePaddingToDefault();

// tf::Vector3 pregrasp_dir;
// tf::vector3MsgToTF(doNegate(handDescription().approachDirection(pickup_goal.arm_name)), pregrasp_dir);
// pregrasp_dir.normalize();

// tf::Vector3 distance_pregrasp_dir = pregrasp_dir*fabs(grasp.desired_approach_distance);

// tf::Transform pre_grasp_trans(tf::Quaternion(0,0,0,1.0), distance_pregrasp_dir);
// tf::Transform pre_grasp_pose = grasp_pose*pre_grasp_trans;
// state->updateKinematicStateWithLinkAt(handDescription().gripperFrame(pickup_goal.arm_name),pre_grasp_pose);

// if(cm->isKinematicStateInCollision(*state)) {
//   ROS_INFO_STREAM("Pre-grasp pose in collision");
//   execution_info.result_.result_code = GraspResult::PREGRASP_IN_COLLISION;
// }

// tf::Vector3 lift_dir;
// tf::vector3MsgToTF(pickup_goal.lift.direction.vector, lift_dir);
// lift_dir.normalize();
// tf::Vector3 distance_lift_dir = lift_dir*fabs(pickup_goal.lift.desired_distance);
// tf::Transform lift_trans(tf::Quaternion(0,0,0,1.0), distance_lift_dir);

// //only works if pre-grasp in robot frame
// tf::Transform lift_pose = lift_trans*grasp_pose;
// state->updateKinematicStateWithLinkAt(handDescription().gripperFrame(pickup_goal.arm_name),lift_pose);

// if(cm->isKinematicStateInCollision(*state)) {
//   ROS_INFO_STREAM("Lift pose in collision");
//   execution_info.result_.result_code = GraspResult::LIFT_IN_COLLISION;
// }

// //now we turn link paddings back on for rest of the evaluation
// //cm->applyLinkPaddingToCollisionSpace(linkPaddingForGrasp(pickup_goal));
// //and also go back to pretty much the original acm
// cm->setAlteredAllowedCollisionMatrix(original_acm);
// cm->disableCollisionsForNonUpdatedLinks(pickup_goal.arm_name);
// group_disable_acm = cm->getCurrentAllowedCollisionMatrix();
// group_disable_acm.changeEntry(pickup_goal.collision_object_name, end_effector_links, true); 
// cm->setAlteredAllowedCollisionMatrix(group_disable_acm);

// //now grasp pose is in world frame
// std_msgs::Header world_header;
// world_header.frame_id = cm->getWorldFrameId();

// //now call ik for grasp
// geometry_msgs::Pose grasp_geom_pose;
// tf::poseTFToMsg(grasp_pose, grasp_geom_pose);
// geometry_msgs::PoseStamped base_link_grasp_pose;
// cm->convertPoseGivenWorldTransform(*state,
//                                   "torso_lift_link",
//                                   world_header,
//                                   grasp_geom_pose,
//                                   base_link_grasp_pose);
// moveit_msgs::Constraints emp;
// sensor_msgs::JointState solution;
// moveit_msgs::MoveItErrorCodes error_code;
// if(!ik_solver_map_[pickup_goal.arm_name]->findConstraintAwareSolution(base_link_grasp_pose.pose,
//                                                                       emp,
//                                                                       state,
//                                                                       solution,
//                                                                       error_code,
//                                                                       false)) {
//   ROS_INFO_STREAM("Grasp out of reach");
//   execution_info.result_.result_code = GraspResult::GRASP_OUT_OF_REACH;
// } 

// std::vector<std::string> grasp_ik_names = solution.name;
// std::vector<double> grasp_ik_solution = solution.position;

// //state will have a good seed state
// geometry_msgs::Pose pre_grasp_geom_pose;
// tf::poseTFToMsg(pre_grasp_pose, pre_grasp_geom_pose);
// geometry_msgs::PoseStamped base_link_pre_grasp_pose;
// cm->convertPoseGivenWorldTransform(*state,
//                                   "torso_lift_link",
//                                   world_header,
//                                   pre_grasp_geom_pose,
//                                   base_link_pre_grasp_pose);

// if(!ik_solver_map_[pickup_goal.arm_name]->findConsistentConstraintAwareSolution(base_link_pre_grasp_pose.pose,
//                                                                                 emp,
//                                                                                 state,
//                                                                                 solution,
//                                                                                 error_code,
//                                                                                 redundancy_,
//                                                                                 consistent_angle_, 
//                                                                                 false)) {
//   ROS_INFO_STREAM("Pre-grasp out of reach");
//   execution_info.result_.result_code = GraspResult::PREGRASP_OUT_OF_REACH;
// } 

// //now we want to go back to the grasp
// std::map<std::string, double> grasp_ik_map;
// for(unsigned int i = 0; i < grasp_ik_names.size(); i++) {
//   grasp_ik_map[grasp_ik_names[i]] = grasp_ik_solution[i];
// }
// state->setKinematicState(grasp_ik_map);

// geometry_msgs::Pose lift_geom_pose;
// tf::poseTFToMsg(lift_pose, lift_geom_pose);
// geometry_msgs::PoseStamped base_link_lift_pose;
// cm->convertPoseGivenWorldTransform(*state,
//                                   "torso_lift_link",
//                                   world_header,
//                                   lift_geom_pose,
//                                   base_link_lift_pose);

// if(!ik_solver_map_[pickup_goal.arm_name]->findConsistentConstraintAwareSolution(base_link_lift_pose.pose,
//                                                                                 emp,
//                                                                                 state,
//                                                                                 solution,
//                                                                                 error_code,
//                                                                                 redundancy_,
//                                                                                 consistent_angle_,
//                                                                                 false)) {
//   ROS_INFO_STREAM("Lift out of reach");
//   execution_info.result_.result_code = GraspResult::LIFT_OUT_OF_REACH;
// } 
  
// tf::Transform base_link_bullet_grasp_pose;
// tf::poseMsgToTF(base_link_grasp_pose.pose, base_link_bullet_grasp_pose);
// //now we need to do interpolated ik
// if(!getInterpolatedIK(pickup_goal.arm_name,
//                       base_link_bullet_grasp_pose,
//                       pregrasp_dir,
//                       grasp.desired_approach_distance,
//                       grasp_ik_solution,
//                       true,
//                       false,
//                       execution_info.approach_trajectory_)) {
//   ROS_INFO_STREAM("No interpolated IK for pre-grasp to grasp");
//   execution_info.result_.result_code = GraspResult::PREGRASP_UNFEASIBLE;
// }

// if(!getInterpolatedIK(pickup_goal.arm_name,
//                       base_link_bullet_grasp_pose,
//                       lift_dir,
//                       pickup_goal.lift.desired_distance,
//                       grasp_ik_solution,
//                       false,
//                       true,
//                       execution_info.lift_trajectory_)) {
//   ROS_INFO_STREAM("No interpolated IK for grasp to lift");
//   execution_info.result_.result_code = GraspResult::LIFT_UNFEASIBLE;
// }

// ROS_INFO_STREAM("Success took " << (ros::WallTime::now()-start));
// execution_info.result_.result_code = GraspResult::SUCCESS;
//}

void GraspEvaluatorFast::testGrasps(const planning_scene::PlanningSceneConstPtr& planning_scene,
                                    const planning_models::KinematicState* seed_state,
                                    const moveit_manipulation_msgs::PickupGoal &pickup_goal,
                                    const std::vector<moveit_manipulation_msgs::Grasp> &grasps,
                                    GraspExecutionInfoVector &execution_info,
                                    bool return_on_first_hit) 
  
{
  ros::WallTime start = ros::WallTime::now();

  std::map<unsigned int, unsigned int> outcome_count;
    
  planning_models::KinematicState state(*seed_state);

  std::map<std::string, double> planning_scene_state_values;
  state.getStateValues(planning_scene_state_values);

  std::string tip_link = planning_scene->getSemanticModel()->getTipLink(pickup_goal.arm_name);
  
  std::string end_effector_group = planning_scene->getSemanticModel()->getEndEffector(pickup_goal.arm_name);
  
  std::vector<std::string> end_effector_links = planning_scene->getSemanticModel()->getGroupLinks(end_effector_group);
  std::vector<std::string> arm_links = planning_scene->getSemanticModel()->getGroupLinks(pickup_goal.arm_name);
  
  collision_detection::AllowedCollisionMatrix original_acm = planning_scene->getAllowedCollisionMatrix();
  collision_detection::AllowedCollisionMatrix group_disable_acm = planning_scene->disableCollisionsForNonUpdatedLinks(pickup_goal.arm_name);

  collision_detection::AllowedCollisionMatrix object_disable_acm = group_disable_acm;
  object_disable_acm.setEntry(pickup_goal.collision_object_name, end_effector_links, true); 
  collision_detection::AllowedCollisionMatrix object_support_disable_acm = object_disable_acm;
  if(pickup_goal.allow_gripper_support_collision)
  {
    if(pickup_goal.collision_support_surface_name == "\"all\"")
    {
      for(unsigned int i = 0; i < end_effector_links.size(); i++){
	object_support_disable_acm.setDefaultEntry(end_effector_links[i], true);
      }
    }
    else{
      ROS_INFO("not all");
      object_support_disable_acm.setEntry(pickup_goal.collision_support_surface_name, end_effector_links, true); 
    }
  }
  collision_detection::AllowedCollisionMatrix object_all_arm_disable_acm = object_disable_acm;
  collision_detection::AllowedCollisionMatrix object_support_all_arm_disable_acm = object_support_disable_acm;
  collision_detection::AllowedCollisionMatrix group_all_arm_disable_acm = group_disable_acm;

  //turning off collisions for the arm associated with this end effector
  for(unsigned int i = 0; i < arm_links.size(); i++) {
    object_all_arm_disable_acm.setDefaultEntry(arm_links[i], true);
    object_support_all_arm_disable_acm.setDefaultEntry(arm_links[i], true);
    group_all_arm_disable_acm.setDefaultEntry(arm_links[i], true);
  }
  
  //first we apply link padding for grasp check
  //cm->applyLinkPaddingToCollisionSpace(linkPaddingForGrasp(pickup_goal));
    
  //setup that's not grasp specific  
  std_msgs::Header target_header;
  target_header.frame_id = pickup_goal.target.reference_frame_id;

  bool in_object_frame = false;
  Eigen::Affine3d obj_pose(Eigen::Affine3d::Identity());

  if(pickup_goal.target.reference_frame_id == pickup_goal.collision_object_name) {
    in_object_frame = true;
    Eigen::Affine3d potential_pose;
    planning_models::poseFromMsg(pickup_goal.target.potential_models[0].pose.pose, potential_pose);
    planning_scene->getTransforms()->transformPose(state,
                                                   pickup_goal.target.potential_models[0].pose.header.frame_id,
                                                   potential_pose,
                                                   obj_pose);
  }
  
  //assumes that whatever is in there is valid
  unsigned int current_size = execution_info.size();
  execution_info.pickup_goal_ = pickup_goal;
  execution_info.grasps_ = grasps;
  execution_info.resize(grasps.size());
  for(unsigned int i = current_size-1; i < execution_info.size(); i++) {
    execution_info[i].result_.result_code = 0;
  }

  Eigen::Vector3d pregrasp_dir(-1.0,0.0,0.0);
  //tf::vector3MsgToTF(doNegate(handDescription().approachDirection(pickup_goal.arm_name)), pregrasp_dir);
  pregrasp_dir.normalize();

  Eigen::Vector3d lift_dir;
  lift_dir.x() = pickup_goal.lift.direction.vector.x;
  lift_dir.y() = pickup_goal.lift.direction.vector.y;
  lift_dir.z() = pickup_goal.lift.direction.vector.z;
  lift_dir.normalize();
  Eigen::Translation3d distance_lift_dir(lift_dir*fabs(pickup_goal.lift.desired_distance));
  Eigen::Affine3d lift_trans(distance_lift_dir*Eigen::Quaterniond::Identity());

  std_msgs::Header world_header;
  world_header.frame_id = planning_scene->getPlanningFrame();
  std::vector<std::string> joint_names = planning_scene->getSemanticModel()->getGroupJoints(pickup_goal.arm_name);

  std::vector<Eigen::Affine3d> grasp_poses(grasps.size());

  //now this is grasp specific
  for(unsigned int i = 0; i < grasps.size(); i++) {

    if(execution_info[i].result_.result_code != 0) {
      ROS_INFO_STREAM("Assuming grasp " << i << " already evaluated");
      continue;
    }

    // ------------- CHECKING GRASP POSE ------------------

    //check whether the grasp pose is ok (only checking hand, not arms)
    //using pre-grasp posture, cause grasp_posture only matters for closing the gripper
    std::map<std::string, double> pre_grasp_joint_vals;    
    for(unsigned int j = 0; j < grasps[i].pre_grasp_posture.name.size(); j++) {
      pre_grasp_joint_vals[grasps[i].pre_grasp_posture.name[j]] = grasps[i].pre_grasp_posture.position[j];
    }
    state.setStateValues(pre_grasp_joint_vals);

    if(!in_object_frame) {
      Eigen::Affine3d grasp_pose;
      planning_models::poseFromMsg(grasps[i].grasp_pose, grasp_pose);
      planning_scene->getTransforms()->transformPose(state,
                                                     target_header.frame_id,
                                                     grasp_pose,
                                                     grasp_poses[i]);
    } else {
      Eigen::Affine3d gp;
      planning_models::poseFromMsg(grasps[i].grasp_pose, gp);
      grasp_poses[i] = obj_pose*gp;
    }
    state.updateStateWithLinkAt(tip_link,grasp_poses[i]);

    collision_detection::CollisionRequest req;
    collision_detection::CollisionResult res;
    //cm->setAlteredAllowedCollisionMatrix(object_support_all_arm_disable_acm);
    execution_info[i].grasp_pose_ = grasp_poses[i];
    planning_scene->checkCollisionUnpadded(req, res, state, object_support_all_arm_disable_acm);
    if(res.collision) {
      execution_info[i].result_.result_code = GraspResult::GRASP_IN_COLLISION;
      outcome_count[GraspResult::GRASP_IN_COLLISION]++;
      continue;
    } 
    
    // ------------- CHECKING PREGRASP POSE ------------------    

    Eigen::Translation3d distance_pregrasp_dir(pregrasp_dir*fabs(grasps[0].desired_approach_distance));    
    Eigen::Affine3d pre_grasp_trans(distance_pregrasp_dir*Eigen::Quaterniond::Identity());
    Eigen::Affine3d pre_grasp_pose = grasp_poses[i]*pre_grasp_trans;
    state.updateStateWithLinkAt(tip_link,pre_grasp_pose);

    res = collision_detection::CollisionResult();
    //cm->setAlteredAllowedCollisionMatrix(group_all_arm_disable_acm);
    planning_scene->checkCollision(req,res,state,
                                   group_all_arm_disable_acm);
    execution_info[i].pregrasp_pose_ = pre_grasp_pose;
    if(res.collision) {
      ROS_DEBUG_STREAM("Pre-grasp in collision");
      execution_info[i].result_.result_code = GraspResult::PREGRASP_IN_COLLISION;
      outcome_count[GraspResult::PREGRASP_IN_COLLISION]++;
      continue;
    }

    // ------------- CHECKING LIFT POSE ------------------    

    moveit_msgs::AttachedCollisionObject att_obj;
    att_obj.link_name = planning_scene->getSemanticModel()->getAttachLink(end_effector_group);
    att_obj.object.operation = moveit_msgs::CollisionObject::ADD;
    att_obj.object.id = pickup_goal.collision_object_name;
    att_obj.touch_links = end_effector_links;

    execution_info[i].attached_object_diff_scene_.reset(new planning_scene::PlanningScene(planning_scene));
    execution_info[i].attached_object_diff_scene_->getCurrentState().updateStateWithLinkAt(tip_link,grasp_poses[i]);
    execution_info[i].attached_object_diff_scene_->processAttachedCollisionObjectMsg(att_obj);
    execution_info[i].attached_object_diff_scene_->getCurrentState().setStateValues(planning_scene_state_values);

    std::map<std::string, double> grasp_joint_vals;    
    for(unsigned int j = 0; j < grasps[i].grasp_posture.name.size(); j++) {
      grasp_joint_vals[grasps[i].grasp_posture.name[j]] = grasps[i].grasp_posture.position[j];
    }
    execution_info[i].attached_object_diff_scene_->getCurrentState().setStateValues(grasp_joint_vals);
    
    Eigen::Affine3d lift_pose = lift_trans*grasp_poses[i];
    execution_info[i].attached_object_diff_scene_->getCurrentState().updateStateWithLinkAt(tip_link,lift_pose);
    
    res = collision_detection::CollisionResult();
    execution_info[i].attached_object_diff_scene_->checkCollision(req,res, execution_info[i].attached_object_diff_scene_->getCurrentState(),
                                                                  object_support_all_arm_disable_acm);
    execution_info[i].lift_pose_ = lift_pose;
    if(res.collision) {
      ROS_DEBUG_STREAM("Lift in collision");
      execution_info[i].result_.result_code = GraspResult::LIFT_IN_COLLISION;
      outcome_count[GraspResult::LIFT_IN_COLLISION]++;
      continue;
    }

    // ------------- CHECKING IK FOR GRASP ------------------        

    //getting back to original state for seed
    state.setStateValues(planning_scene_state_values);

    //adjusting planning scene state for pre-grasp
    std::map<std::string, double> pre_grasp_values;
    for(unsigned int j = 0; j < grasps[i].pre_grasp_posture.name.size(); j++) {
      pre_grasp_values[grasps[i].pre_grasp_posture.name[j]] = grasps[i].pre_grasp_posture.position[j];
    }
    state.setStateValues(pre_grasp_values);
    
    //now call ik for grasp
    
    Eigen::Affine3d base_link_world_pose = 
      state.getLinkState(planning_scene->getSemanticModel()->getBaseLink(pickup_goal.arm_name))->getGlobalLinkTransform();

    Eigen::Affine3d base_link_grasp_pose_e = base_link_world_pose.inverse()*grasp_poses[i];
    geometry_msgs::Pose base_link_grasp_pose;
    planning_models::msgFromPose(base_link_grasp_pose_e, base_link_grasp_pose);
    
    moveit_msgs::Constraints emp;
    sensor_msgs::JointState solution;
    moveit_msgs::MoveItErrorCodes error_code;
    ROS_DEBUG_STREAM("X y z " << base_link_grasp_pose.position.x << " " 
                     << base_link_grasp_pose.position.y << " " 
                     << base_link_grasp_pose.position.z);
    if(!constraint_aware_solver_map_[pickup_goal.arm_name]->findConstraintAwareSolution(base_link_grasp_pose,
                                                                                        emp,
                                                                                        &state,
                                                                                        planning_scene,
                                                                                        object_support_disable_acm,
                                                                                        solution,
                                                                                        error_code,
                                                                                        false,
                                                                                        true)) {
      ROS_DEBUG_STREAM("Grasp out of reach");
      execution_info[i].result_.result_code = GraspResult::GRASP_OUT_OF_REACH;
      outcome_count[GraspResult::GRASP_OUT_OF_REACH]++;
      continue;
    }

    // ------------- CHECKING INTERPOLATED IK FROM PREGRASP TO GRASP ------------------        
      
    std::map<std::string, double> ik_map_pre_grasp;
    std::map<std::string, double> ik_map_grasp;
    for(unsigned int j = 0; j < joint_names.size(); j++) {
      ik_map_pre_grasp[joint_names[j]] = solution.position[j];
    } 
    ik_map_grasp = ik_map_pre_grasp;
    
    for(unsigned int j = 0; j < grasps[i].pre_grasp_posture.name.size(); j++) {
      ik_map_pre_grasp[grasps[i].pre_grasp_posture.name[j]] = grasps[i].pre_grasp_posture.position[j];
    }
    for(unsigned int j = 0; j < grasps[i].grasp_posture.name.size(); j++) {
      ik_map_grasp[grasps[i].grasp_posture.name[j]] = grasps[i].grasp_posture.position[j];
    }
    
    state.setStateValues(ik_map_pre_grasp);      
    execution_info[i].approach_trajectory_.joint_names = joint_names;
    //now we need to do interpolated ik
    if(!getInterpolatedIK(pickup_goal.arm_name,
                          planning_scene,
                          object_support_disable_acm,
                          base_link_grasp_pose,
                          pregrasp_dir,
                          grasps[i].desired_approach_distance,
                          solution.position,
                          true,
                          false,
                          true,
                          &state,
                          execution_info[i].approach_trajectory_)) {
      ROS_DEBUG_STREAM("No interpolated IK for pre-grasp to grasp");
      execution_info[i].result_.result_code = GraspResult::PREGRASP_UNFEASIBLE;
      outcome_count[GraspResult::PREGRASP_UNFEASIBLE]++;
      continue;
    }
    
    ROS_DEBUG_STREAM("Last approach point is " <<
                     execution_info[i].approach_trajectory_.points.back().positions[0] << " " << 
                     execution_info[i].approach_trajectory_.points.back().positions[1] << " " <<
                     execution_info[i].approach_trajectory_.points.back().positions[2] << " " <<
                     execution_info[i].approach_trajectory_.points.back().positions[3] << " " <<
                     execution_info[i].approach_trajectory_.points.back().positions[4] << " " <<
                     execution_info[i].approach_trajectory_.points.back().positions[5] << " " <<
                     execution_info[i].approach_trajectory_.points.back().positions[6]);
    
    // ------------- CHECKING INTERPOLATED IK FROM GRASP TO LIFT ------------------        o

    execution_info[i].attached_object_diff_scene_->getCurrentState().setStateValues(ik_map_grasp);
    execution_info[i].lift_trajectory_.joint_names = joint_names;
    //TODO - figure out if we need to muck with allowed collision matrix for the diff scene
    if(!getInterpolatedIK(pickup_goal.arm_name,
                          execution_info[i].attached_object_diff_scene_,
                          object_support_disable_acm,
                          base_link_grasp_pose,
                          lift_dir,
                          pickup_goal.lift.desired_distance,
                          solution.position,
                          false,
                          true,
                          true,
                          &execution_info[i].attached_object_diff_scene_->getCurrentState(),
                          execution_info[i].lift_trajectory_)) {
      ROS_DEBUG_STREAM("No interpolated IK for grasp to lift");
      execution_info[i].result_.result_code = GraspResult::LIFT_UNFEASIBLE;
      outcome_count[GraspResult::LIFT_UNFEASIBLE]++;
      continue;
    }

    ROS_DEBUG_STREAM("First lift point is " <<
                     execution_info[i].lift_trajectory_.points.front().positions[0] << " " << 
                     execution_info[i].lift_trajectory_.points.front().positions[1] << " " << 
                     execution_info[i].lift_trajectory_.points.front().positions[2] << " " <<
                     execution_info[i].lift_trajectory_.points.front().positions[3] << " " <<
                     execution_info[i].lift_trajectory_.points.front().positions[4] << " " <<
                     execution_info[i].lift_trajectory_.points.front().positions[5] << " " <<
                     execution_info[i].lift_trajectory_.points.front().positions[6]);
    
    if(execution_info[i].approach_trajectory_.points.empty()) {
      ROS_WARN_STREAM("No result code and no points in approach trajectory");
      continue;
    }

    // ------------- CHECKING PREGRASP OK FOR PLANNING ------------------            

    std::map<std::string, double> pre_grasp_ik = ik_map_pre_grasp;
    for(unsigned int j = 0; j < joint_names.size(); j++) {
      pre_grasp_ik[joint_names[j]] = execution_info[i].approach_trajectory_.points[0].positions[j];
    } 
    state.setStateValues(pre_grasp_ik);
    //the start of the approach needs to be collision-free according to the default collision matrix
    //cm->setAlteredAllowedCollisionMatrix(group_disable_acm);
    res = collision_detection::CollisionResult();
    planning_scene->checkCollision(req, res, state, group_disable_acm);
    if(res.collision) {
      ROS_DEBUG_STREAM("Final pre-grasp check failed");
      execution_info[i].result_.result_code = GraspResult::PREGRASP_OUT_OF_REACH;
      outcome_count[GraspResult::PREGRASP_OUT_OF_REACH]++;
      continue;
    }
    
    if(execution_info[i].lift_trajectory_.points.empty()) {
      ROS_WARN_STREAM("No result code and no points in lift trajectory");
      continue;
    }    

    // ------------- CHECKING LIFT OK FOR PLANNING ------------------            

    std::map<std::string, double> lift_ik = ik_map_pre_grasp;
    for(unsigned int j = 0; j < joint_names.size(); j++) {
      lift_ik[joint_names[j]] = execution_info[i].lift_trajectory_.points.back().positions[j];
    } 
    execution_info[i].attached_object_diff_scene_->getCurrentState().setStateValues(lift_ik);
    res = collision_detection::CollisionResult();
    execution_info[i].attached_object_diff_scene_->checkCollision(req, res, execution_info[i].attached_object_diff_scene_->getCurrentState());
    if(res.collision) {
      ROS_DEBUG_STREAM("Final lift check failed");
      execution_info[i].result_.result_code = GraspResult::LIFT_OUT_OF_REACH;
      outcome_count[GraspResult::LIFT_OUT_OF_REACH]++;
      continue;
    } else {
      ROS_INFO_STREAM("Everything successful");
      execution_info[i].result_.result_code = GraspResult::SUCCESS;
      execution_info.resize(i+1);
      outcome_count[GraspResult::SUCCESS]++;
      if(return_on_first_hit) {
        break;
      }
    }
  }
  for(std::map<unsigned int, unsigned int>::iterator it = outcome_count.begin();
      it != outcome_count.end();
      it++) {
    ROS_INFO_STREAM("Outcome " << it->first << " count " << it->second);
  }
  ROS_INFO_STREAM("Took " << (ros::WallTime::now()-start).toSec());
  return;
}

} //namespace grasp_place_evaluation
